#ifndef MESSAGING_H
#define MESSAGING_H

#include <zmq.hpp>
#include <SDL2/SDL.h>
#include "Configuration.hpp"
#include "json.hpp"
#include "display.hpp"
#include <vector>

namespace Messaging
{
    class Messaging
    {
        public:
            uint32_t displaySingleImage;
            uint32_t displayDefaultImage;
            uint32_t playVideo;
            explicit Messaging(const Configuration::ConfigData &configuration);
            void handle_message();
        private:
            zmq::context_t m_context;
            zmq::socket_t m_sub;
            int m_NUM_EVENTS = 3;
            
    };
}

#endif