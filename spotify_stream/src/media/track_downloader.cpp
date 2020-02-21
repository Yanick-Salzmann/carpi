#include "track_downloader.hpp"
#include "media_source.hpp"
#include <thread>
#include <filesystem>
#include <common_utils/error.hpp>
#include <common_utils/string.hpp>
#include <utility>
#include <dirent.h>

namespace carpi::spotify::media {
    LOGGER_IMPL(track_downloader);

    track_downloader::track_downloader(std::shared_ptr<MediaSource> source, track_metadata meta_data) :
            _meta_data{std::move(meta_data)},
            _source{std::move(source)} {
        _source->seek_position(0);
    }

    std::future<void> track_downloader::download_to_folder(const std::string &folder) {
        return std::async(std::launch::async, [=]() {
            _album_cover = download_album_image();

            const auto file_path = output_path(folder);
            const auto sub_process = launch_converter(file_path);

            auto input_reader = create_input_thread(sub_process.stdin_pipe);
            auto stdout_reader = create_output_reader(sub_process.stdout_pipe, [=](auto str) { log->info(str); });
            auto stderr_reader = create_output_reader(sub_process.stderr_pipe, [=](auto str) { log->info(str); });

            join_threads(input_reader, stdout_reader, stderr_reader);
        });
    }

    std::string track_downloader::output_path(const std::string &base_folder) {
        const auto folder = fmt::format("{}/{}", base_folder, _meta_data.artist);
        auto* dir = opendir(folder.c_str());
        if(dir) {
            closedir(dir);
        } else {
            mkdir(folder.c_str(), 0755);
        }

        return fmt::format("{}/{}/{}.mp3", base_folder, _meta_data.artist, _meta_data.title);
    }

    utils::SubProcess track_downloader::launch_converter(const std::string &output_file) {
        std::vector<std::string> args{
                "ffmpeg",
                "-f", "mp4",
                "-i", "-"
        };

        put_metadata(args);

        args.insert(args.end(), {
                "-id3v2_version", "3",
                output_file
        });

        return utils::launch_subprocess("ffmpeg", args);
    }

    std::thread track_downloader::create_input_thread(int stdin_pipe) {
        return std::thread{
                [=]() {
                    FILE* out_file = fopen("output.mp4", "wb");

                    auto chunk = _source->read_next_data();
                    auto is_alive = true;
                    while (is_alive && !chunk.empty()) {
                        fwrite(chunk.data(), 1, chunk.size(), out_file);
                        std::size_t num_written{0};
                        while (num_written < chunk.size()) {
                            const auto cur_write = write(stdin_pipe, chunk.data() + num_written,
                                                         chunk.size() - num_written);
                            if (cur_write <= 0) {
                                is_alive = false;
                                break;
                            }

                            num_written += cur_write;
                        }

                        chunk = _source->read_next_data();
                    }

                    fclose(out_file);
                    close(stdin_pipe);
                }
        };
    }

    std::thread
    track_downloader::create_output_reader(int pipe, const std::function<void(const std::string &)> &output) {
        return std::thread{
                [=]() {
                    std::vector<char> data_buffer(16384);
                    auto num_read = read(pipe, data_buffer.data(), data_buffer.size() - 1);
                    while (num_read > 0) {
                        data_buffer[num_read] = '\0';
                        output(data_buffer.data());
                        num_read = read(pipe, data_buffer.data(), data_buffer.size() - 1);
                    }
                }
        };
    }

    std::shared_ptr<std::string> track_downloader::download_album_image() {
        if (_meta_data.album_image.empty()) {
            return {};
        }

        char buffer[FILENAME_MAX]{"carpi_spotify_XXXXXXXX"};
        const auto ret = tmpnam_r(buffer);
        if(!ret) {
            log->error("Error creating temporary file for album cover: {}", utils::error_to_string());
            return {};
        }

        auto f = fopen(buffer, "wb");

        if (f == nullptr) {
            log->warn("Error opening temp file for album cover download: {}", utils::error_to_string());
            return {};
        }

        std::shared_ptr<FILE> file_ptr(f, [](FILE *f) {
            fclose(f);
        });

        const auto response = _client.execute(net::http_request{"GET", _meta_data.album_image});
        const auto &body = response.body();

        if (response.status_code() != 200) {
            log->warn("Error downloading album image from {}. Response: {} {} (body {})",
                      response.status_code(), response.status_text(), utils::bytes_to_utf8(body));
            return {};
        }

        if (fwrite(body.data(), 1, body.size(), f) != body.size()) {
            log->warn("Error writing album image to file: {}", utils::error_to_string());
            return {};
        }

        return std::shared_ptr<std::string>(new std::string((const char *) buffer), [](std::string *ptr) {
            if (remove(ptr->c_str()) < 0) {
                log->warn("Error removing temporary file {} for album cover: {}", ptr->c_str(),
                          utils::error_to_string());
            }

            delete ptr;
        });
    }

    std::string track_downloader::create_metadata(const std::string &key, const std::string &value) {
        return fmt::format("{}={}", key, value);
    }

    void track_downloader::put_metadata(std::vector<std::string> &args) {
        if (_album_cover) {
            args.insert(args.end(), {
                    "-i", *_album_cover,
                    "-map", "0:0",
                    "-map", "1:0",
                    "-metadata", "comment=\"Cover (front)\""
            });
        }

        put_non_empty_meta_data(args, "title", _meta_data.title);
        put_non_empty_meta_data(args, "artist", _meta_data.artist);
        put_non_empty_meta_data(args, "album", _meta_data.album);
        put_non_empty_meta_data(args, "genre", _meta_data.genre);
        put_non_empty_meta_data(args, "year", std::to_string(_meta_data.year));
    }

    void track_downloader::put_non_empty_meta_data(std::vector<std::string> &args, const std::string &key,
                                                   const std::string &value) {
        if (value.empty()) {
            return;
        }

        args.insert(args.end(), {"-metadata", create_metadata(key, value)});
    }

    track_metadata track_metadata::from_json(const nlohmann::json &metadata) {
        return track_metadata{
                .title = metadata["name"],
                .artist = metadata["artist"],
                .album = metadata["album"],
                .album_image = metadata["album_image"],
                .year = metadata["year"],
                .genre = metadata["genre"]
        };
    }
}