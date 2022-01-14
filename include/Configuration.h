#pragma once
#include <string>
#include <exception>
#include "argh.h"
#include "Yaml.hpp"
#include <memory>

namespace Configuration
{
    class ConfigData
    {
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

    public:
        ConfigData(int argc, char *argv[]);
        ~ConfigData();

        void ParseConfigFile();
    };
}