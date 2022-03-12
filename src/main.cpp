#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include "display.hpp"
#include <vector>
#include <numeric>
#include <zmq.hpp>
#include "json.hpp"

using json = nlohmann::json;
int main(int argc, char *argv[])
{
    Configuration::ConfigData configuration{argc, argv};
    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(configuration.width), static_cast<unsigned int>(configuration.height)), "Video Wall Client", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    Display::Display display{configuration};

    zmq::context_t context;
    zmq::socket_t sub(context, zmq::socket_type::sub);
    sub.connect(configuration.zmq_server);
    sub.set(zmq::sockopt::subscribe, "");

    while (window.isOpen())
    {
        display.Refresh(window);
        zmq::message_t msg;
            auto res = sub.recv(msg, zmq::recv_flags::dontwait);
            if (res)
            {
                try
                {
                    json j = json::parse(msg.to_string());
                    if (j["command"].get<std::string>() == std::string("display_image"))
                    {
                        display.DisplaySingleImage(j["location"]);
                    }
                }
                catch (nlohmann::detail::parse_error const &e)
                {
                    std::cerr << "Error parsing json" << '\n';
                    std::cerr << "Failed string was: " << msg.to_string() << '\n';
                    continue;
                }
            }
    }
    
    return EXIT_SUCCESS;

}
