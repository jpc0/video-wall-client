#pragma once
#include <string>
#include <exception>
#include "argh.h"
#include "Yaml.hpp"
#include <memory>
#include <crossguid/guid.hpp>

namespace Configuration
{
    class ConfigData
    {
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
        std::string image_location;
        std::string zmq_server;
        xg::Guid guid;

    public:
        ConfigData(int argc, char *argv[]);
        ~ConfigData();

        void ParseConfigFile();
    };
}