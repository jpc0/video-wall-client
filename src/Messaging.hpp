#ifndef MESSAGING_H
#define MESSAGING_H

#include "main.hpp"
#include <zmq.hpp>
#include <SDL2/SDL.h>
#include "Configuration.hpp"
#include "json.hpp"
#include "display.hpp"
#include <vector>
#include <thread>

namespace Messaging
{
    class Messaging
    {
        public:
            uint32_t playVideo;
            explicit Messaging(const Configuration::ConfigData &configuration, const CustomMessages &customMessages);
            void stopThread();
            static void handle_message(uint32_t displaySingleImage, uint32_t displayDefaultImage);
        private:
            zmq::context_t m_context;
            zmq::socket_t m_sub;
            std::jthread m_messageHandler;
    };
}

#endif