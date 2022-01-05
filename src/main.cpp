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
        sub.connect("tcp://192.168.1.39:5678");
        sub.set(zmq::sockopt::subscribe, "");
        // Main loop
        while (!display.ShouldExit())
        {
            zmq::message_t msg;
            auto res = sub.recv(msg, zmq::recv_flags::dontwait);
            if (res)
            {
                json j = json::parse(static_cast<char *>(msg.data()));
                if (j["command"].get<std::string>() == std::string("display_image"))
                {
                    display.DisplaySingleImage(j["location"]);
                }
            }

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(5ms);
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