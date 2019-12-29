#include <iostream>
#include <net_utils/ssl_socket.hpp>
#include <net_utils/url_parser.hpp>
#include <net_utils/websocket_client.hpp>
#include <net_utils/http_client.hpp>

namespace carpi {
    int main(int argc, const char* argv[]) {
        //net::WebsocketClient websocket{"wss://gew-dealer.spotify.com/"
        //                               "?access_token=BQAi1CCNKMChPYt-qvPjRS7mX__T941ksRVmlWAm3WQQ90in8CNOzMIq5RS8oEnL5FtGYAHXr2TYSzyFKJhT4yEAwfHQqFfBhWiwBiz4hjAT642Obr4BojD1rSh_-WM5B4Ahd3VcC8vO4UYgqhAyc7ABaYPbvzkykCVzzi24koArIwrr5AEPGJ0"};

        net::HttpClient client{};
        net::HttpRequest request{"GET", "https://apresolve.spotify.com/?type=dealer"};
        auto response = client.execute(request);

        std::cout << response.to_string() << std::endl;

        std::cin.get();
        return 0;
    }
}

int main(int argc, const char* argv[]) {
    return carpi::main(argc, argv);
}