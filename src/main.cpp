#include <iostream>
#include <string>
#include <sstream>
#include "Configuration.h"
#include "Display.h"
#include <thread>
#include <chrono>
#include <zmq.hpp>
#include <json.hpp>

using json = nlohmann::json;
int main(int argc, char *argv[])
{
    try
    {
        // Base Setup

        Configuration::ConfigData configuration{argc, argv};
        Display::Display display{configuration};

        // ZeroMQ Setup, this should be refactored
        // TODO: Refactor

        zmq::context_t context;
        zmq::socket_t sub(context, zmq::socket_type::sub);
        sub.connect(configuration.zmq_server);
        sub.set(zmq::sockopt::subscribe, "");
        // Main loop
        Uint32 start_time, frame_time;
        float fps;
        start_time = SDL_GetTicks();
        while (!display.ShouldExit())
        {
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
                catch (nlohmann::detail::parse_error)
                {
                    std::cerr << "Error parsing json" << '\n';
                    std::cerr << "Failed string was: " << msg.to_string() << '\n';
                    continue;
                }
            }

            // using namespace std::chrono_literals;
            // std::this_thread::sleep_for(6ms);
            frame_time = SDL_GetTicks() - start_time;
            fps = (frame_time > 0) ? 1000.0f / frame_time : 0.0f;
            // std::cout << "Frame Time: " << frame_time << '\n';
            // std::cout << "FPS: " << fps << '\n';
            start_time = SDL_GetTicks();
        }

        return 0;
    }
    catch (Configuration::Quit &e)
    {
        return 0;
    }

    catch (INITFAIL &e)
    {
        std::cout << e.what() << std::endl;
    }
}
