#include <spdlog/spdlog.h>
#include "display.hpp"
#include <vector>
#include <numeric>
#include <zmq.hpp>
#include "json.hpp"
#include <SDL2/SDL.h>

using json = nlohmann::json;
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_VideoInit(nullptr);
    Configuration::ConfigData configuration{argc, argv};
    Display::Display display{configuration};

    zmq::context_t context;
    zmq::socket_t sub(context, zmq::socket_type::sub);
    sub.connect(configuration.zmq_server);
    sub.set(zmq::sockopt::subscribe, "");

    while (!configuration.ShouldQuit)
    {
        display.Refresh(configuration);
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

    SDL_Quit();
    return EXIT_SUCCESS;

}
