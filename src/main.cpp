#include <spdlog/spdlog.h>
#include "display.hpp"
#include "Messaging.hpp"
#include <vector>
#include <numeric>
#include <zmq.hpp>
#include "json.hpp"
#include <SDL2/SDL.h>
#include "Video.hpp"

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_VideoInit(nullptr);
    // Video::Video video{};
    Configuration::ConfigData configuration{argc, argv};
    Display::Display display{configuration};
    Messaging::Messaging messaging_handler(configuration); 
    bool shouldQuit = false;
   
    while (!shouldQuit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    shouldQuit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                    shouldQuit = true;
                    
                    }
                    break;
                default:
                    if (event.type == messaging_handler.displaySingleImage)
                       display.DisplaySingleImage(std::bit_cast<const char*>(event.user.data1));
                    if (event.type == messaging_handler.displayDefaultImage)
                       display.DisplayDefaultImage();
                    break;
            }
        }

        display.Refresh();
        messaging_handler.handle_message();
    }

    SDL_Quit();
    return EXIT_SUCCESS;

}
