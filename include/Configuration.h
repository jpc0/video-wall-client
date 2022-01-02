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
        int l_bezel;
        int r_bezel;
        int t_bezel;
        int b_bezel;
        int h_index;
        int v_index;
        int h_screens;
        int v_screens;
        int width;
        int height;
        int total_width;
        int total_height;
        std::string image_location;
    };
}