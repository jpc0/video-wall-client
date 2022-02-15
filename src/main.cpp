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
                    if (j["command"].get<std::string>() == std::string("adjust_color"))
                    {
                        if (j["attribute"].get<std::string>() == std::string("brightness"))
                        {
                            if (j["channel"].get<std::string>() == std::string("all"))
                            {
                                std::cout << display.AdjustBrightness(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("red"))
                            {
                                std::cout << display.AdjustBrightnessr(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("green"))
                            {
                                std::cout << display.AdjustBrightnessg(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("blue"))
                            {
                                std::cout << display.AdjustBrightnessb(j["value"].get<float>()) << '\n';
                            }
                        }
                        if (j["attribute"].get<std::string>() == std::string("contrast"))
                        {
                            if (j["channel"].get<std::string>() == std::string("all"))
                            {
                                std::cout << display.AdjustContrast(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("red"))
                            {
                                std::cout << display.AdjustContrastr(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("green"))
                            {
                                std::cout << display.AdjustContrastg(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("blue"))
                            {
                                std::cout << display.AdjustContrastb(j["value"].get<float>()) << '\n';
                            }
                        }
                        if (j["attribute"].get<std::string>() == std::string("gamma"))
                        {
                            if (j["channel"].get<std::string>() == std::string("all"))
                            {
                                std::cout << display.AdjustGamma(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("red"))
                            {
                                std::cout << display.AdjustGammar(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("green"))
                            {
                                std::cout << display.AdjustGammag(j["value"].get<float>()) << '\n';
                            }
                            if (j["channel"].get<std::string>() == std::string("blue"))
                            {
                                std::cout << display.AdjustGammab(j["value"].get<float>()) << '\n';
                            }
                        }
                    }
                }
                catch (nlohmann::detail::parse_error)
                {
                    std::cerr << "Error parsing json" << '\n';
                    std::cerr << "Failed string was: " << msg.to_string() << '\n';
                    continue;
                }
            }
        }

        return 0;
    }
    catch (INITFAIL &e)
    {
        std::cout << e.what() << std::endl;
    }
}
