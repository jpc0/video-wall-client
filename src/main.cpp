#include <spdlog/spdlog.h>
#include <chrono>
#include "display.hpp"
#include "Command.hpp"
#include <vector>
#include <numeric>
#include <zmq.hpp>
#include <SDL2/SDL.h>
#include "Video.hpp"
#include "main.hpp"
#include "MessageHandler/MessageHandler.hpp"

constexpr int m_NUM_EVENTS = 3;

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_VideoInit(nullptr);
    uint32_t CustomEventStart = SDL_RegisterEvents(m_NUM_EVENTS);
    CustomMessages customMessages{
        CustomEventStart,
        CustomEventStart +1,
        CustomEventStart +2
    };
    [[ maybe_unused ]] MessageHandler messageHandler{};
    // Video::Video video{customMessages};
    Configuration::ConfigData configuration{argc, argv};
    Display::Display display{configuration, customMessages};
    Command::Command messaging_handler(configuration, customMessages); 
    bool shouldQuit = false;
    auto start_time = std::chrono::high_resolution_clock().now();
    while (!shouldQuit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    shouldQuit = true;
                    messaging_handler.stopThread();
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                    shouldQuit = true;
                    messaging_handler.stopThread();
                    }
                    break;
                default:
                    if (event.type == customMessages.displaySingleImage)
                       display.DisplaySingleImage(reinterpret_cast<const char*>(event.user.data1));
                    if (event.type == customMessages.displayDefaultImage)
                       display.DisplayDefaultImage();
                    // if (event.type == video.videoReady)
                    //    display.PrepVideo(reinterpret_cast<Display::VideoType*>(event.user.data1));
                    break;
            }
        }

        display.Refresh();
        messageHandler.handleMessages();
        // messaging_handler.handle_message();
        auto this_frame = std::chrono::high_resolution_clock().now() - start_time;
    }

    SDL_Quit();
    return EXIT_SUCCESS;

}
