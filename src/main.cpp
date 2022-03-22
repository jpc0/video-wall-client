#include <spdlog/spdlog.h>
#include "display.hpp"
#include "Messaging.hpp"
#include <vector>
#include <numeric>
#include <zmq.hpp>
#include "json.hpp"
#include <SDL2/SDL.h>

using json = nlohmann::json;
int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_VideoInit(nullptr);
    Configuration::ConfigData configuration{argc, argv};
    Display::Display display{configuration};
    Messaging::Messaging messaging_handler(configuration); 

    while (!configuration.ShouldQuit)
    {
        display.Refresh(configuration);
        messaging_handler.handle_message();
    }

    SDL_Quit();
    return EXIT_SUCCESS;

}
