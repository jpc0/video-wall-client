#include "display.hpp"
#include "Command.hpp"
#include <zmq.hpp>
#include <SDL2/SDL.h>
#include "main.hpp"
#include "MessageHandler/MessageHandler.hpp"

constexpr int m_NUM_EVENTS = 3;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      spdlog::info("Failed to initialize SDL: \n{}", SDL_GetError());
      exit(EXIT_FAILURE);
    }
    if (SDL_VideoInit(nullptr) < 0)
    {
      spdlog::info("Failed to initialize SDL Video Engine: \n{}", SDL_GetError());
      exit(EXIT_FAILURE);
    }
    uint32_t CustomEventStart = SDL_RegisterEvents(m_NUM_EVENTS);
    CustomMessages customMessages{
        CustomEventStart,
        CustomEventStart +1,
        CustomEventStart +2
    };
    [[ maybe_unused ]] MessageHandler messageHandler{};
    Configuration::ConfigData configuration{argc, argv};
    Display::Display display{configuration, customMessages};
    Command::Command messaging_handler(configuration);
    bool shouldQuit = false;
    std::shared_ptr<MessageQueue> DisplayQueue = MessageHandler::registerReceiver(Destination::DisplayMessage);
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
        if (auto msg = DisplayQueue->receive())
          display.DisplaySingleImage(msg.value().message);
        display.Refresh();
        MessageHandler::handleMessages();
    }

    SDL_Quit();
    return EXIT_SUCCESS;

}
