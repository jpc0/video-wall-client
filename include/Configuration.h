#pragma once
#include <string>
#include <exception>

namespace Configuration
{
    struct Quit : public std::exception
    {
        const char *what() const throw()
        {
            return 0;
        }
    };
    class ConfigData
    {
    public:
        ConfigData(const int argc, char *argv[]);
        ~ConfigData() = default;

    public:
        std::string config_path;
        float l_bezel;
        float r_bezel;
        float t_bezel;
        float b_bezel;
        float h_index;
        float v_index;
        float h_screens;
        float v_screens;
        float width;
        float height;
        std::string image_location;
        std::string zmq_server;
    };
}