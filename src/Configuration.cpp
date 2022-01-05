#include "Configuration.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <string>
#include <iostream>

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

namespace Configuration
{
    ConfigData::ConfigData(const int argc, char *argv[])
    {
        // clang-format off
        boost::program_options::options_description generic("Generic options");
        generic.add_options()
            ("help", "Show help message")
            ("config, c", boost::program_options::value<std::string>(&config_path)->default_value("../config.cfg"), 
                                        "config file location");

        boost::program_options::options_description config("Configuration");
        config.add_options()
            ("l_bezel", boost::program_options::value<float>(&l_bezel)->default_value(0), "left bezel in px")
            ("r_bezel", boost::program_options::value<float>(&r_bezel)->default_value(0), "right bezel in px")
            ("t_bezel", boost::program_options::value<float>(&t_bezel)->default_value(0), "top bezel in px")
            ("b_bezel", boost::program_options::value<float>(&b_bezel)->default_value(0), "bottom bezel in px")
            ("h_index", boost::program_options::value<float>(&h_index)->default_value(0), "horizontal index of the screen, 0 based from bottom left screen")
            ("v_index", boost::program_options::value<float>(&v_index)->default_value(0), "verticle index of the screen, 0 based from bottom left screen")
            ("h_screens", boost::program_options::value<float>(&h_screens)->default_value(0), "Number of screens horizontally")
            ("v_screens", boost::program_options::value<float>(&v_screens)->default_value(0), "Number of screens vertically")
            ("width", boost::program_options::value<float>(&width)->default_value(1280), "width of each screen in px")
            ("height", boost::program_options::value<float>(&height)->default_value(720), "height of each screen in px")
            ("image_location", boost::program_options::value<std::string>(&image_location)->default_value(""), "location of default image")
            ("zmq_server", boost::program_options::value<std::string>(&zmq_server)->default_value("tcp://127.0.0.1:7000"), "location of zmq_server")
        ;

        boost::program_options::options_description cmdline_options;
        cmdline_options.add(generic).add(config);

        boost::program_options::options_description config_file_options;
        config_file_options.add(config);

        boost::program_options::options_description visible("Allowed options");
            visible.add(generic).add(config);

        // clang-format on

        boost::program_options::variables_map args;
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).run(), args);
        boost::program_options::notify(args);

        std::ifstream ifs(config_path.c_str());
        if (!ifs)
        {
            std::cout << "Can not open config file: " << config_path << "\n";
            throw Quit{};
        }
        else
        {
            boost::program_options::store(boost::program_options::parse_config_file(ifs, config_file_options), args);
            boost::program_options::notify(args);
        }

        if (args.count("help"))
        {
            std::cout << visible << std::endl;
            throw Quit{};
        }
    }

}
