#include "Configuration.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <crossguid/guid.hpp>

namespace Configuration
{
    ConfigData::ConfigData(const int argc, char *argv[])
    {
        argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
        cmdl({"-c", "--config"}, "../../config.yaml") >> config_path;
        ParseConfigFile();
        guid = xg::newGuid();
        if (!guid.isValid())
        {
            std::cerr << "Unable to generate GUID" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    ConfigData::~ConfigData(){};

    void ConfigData::ParseConfigFile()
    {
        Yaml::Node configfile;
        Yaml::Parse(configfile, config_path.c_str());
        l_bezel = configfile["l_bezel"].As<int>();
        r_bezel = configfile["r_bezel"].As<int>();
        t_bezel = configfile["t_bezel"].As<int>();
        b_bezel = configfile["b_bezel"].As<int>();
        h_index = configfile["h_index"].As<int>();
        v_index = configfile["v_index"].As<int>();
        h_screens = configfile["h_screens"].As<int>();
        v_screens = configfile["v_screens"].As<int>();
        width = configfile["width"].As<int>();
        height = configfile["height"].As<int>();
        image_location = configfile["image_location"].As<std::string>();
        zmq_server = configfile["zmq_server"].As<std::string>();
    }

}