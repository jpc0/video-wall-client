#ifndef MESSAGING_H
#define MESSAGING_H

#include <zmq.h>
#include <SDL2/SDL.h>
#include "Configuration.hpp"

namespace Messaging
{
    class Messaging
    {
        public:
            Messaging(const Configuration::ConfigData &configuration);
            void handle_message();
        private:
            zmq::context_t m_context;
            zmq::socket_t m_sub;
    };
}

#endif