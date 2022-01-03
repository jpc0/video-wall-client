#include <iostream>
#include <string>
#include <sstream>
#include "Configuration.h"
#include "Display.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
    try
    {
        Configuration::ConfigData configuration{argc, argv};
        Display display{configuration};
        display.DisplaySingleImage(configuration);
        while (!display.ShouldExit())
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(2000ms);
        }
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