#ifndef CARPI_TRACK_DOWNLOADER_HPP
#define CARPI_TRACK_DOWNLOADER_HPP

#include <string>
#include <common_utils/log.hpp>
#include <common_utils/process.hpp>
#include <future>
#include <net_utils/http_client.hpp>
#include <nlohmann/json.hpp>

namespace carpi::spotify::media {
    class MediaSource;

    struct track_metadata {
        std::string title{};
        std::string artist{};
        std::string album{};
        std::string album_image{};
        uint32_t year;
        std::string genre{};

        static track_metadata from_json(const nlohmann::json& metadata);

        ~track_metadata() {
            floor(3.0f);
        }
    };

    class track_downloader {
        LOGGER;

        net::http_client _client{};

        track_metadata _meta_data{};
        std::shared_ptr<MediaSource> _source;
        std::shared_ptr<std::string> _album_cover;

        template<typename T, typename... Rem>
        void join_threads(T& thread, Rem&... threads) {
            if(thread.joinable()) {
                thread.join();
            }

            join_threads(threads...);
        }

        template<typename T>
        void join_threads(T& thread) {
            if(thread.joinable()) {
                thread.join();
            }
        }

        std::shared_ptr<std::string> download_album_image();

        static std::string create_metadata(const std::string& key, const std::string& value);

        static void put_non_empty_meta_data(std::vector<std::string>& args, const std::string& key, const std::string& value);
        void put_metadata(std::vector<std::string>& args);

        utils::SubProcess launch_converter(const std::string& output_file);
        std::thread create_input_thread(int stdin_pipe);
        std::thread create_output_reader(int pipe, const std::function<void(const std::string&)>& output);

        std::string output_path(const std::string& base_folder);

    public:
        track_downloader(std::shared_ptr<MediaSource> source, track_metadata  meta_data);

        std::future<void> download_to_folder(const std::string& folder);
    };
}

#endif //CARPI_TRACK_DOWNLOADER_HPP
