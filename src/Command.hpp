#ifndef MESSAGING_H
#define MESSAGING_H

#include "main.hpp"
#include <zmq.hpp>
#include <SDL2/SDL.h>
#include "Configuration.hpp"
#include <nlohmann/json.hpp>
#include "display.hpp"
#include <vector>
#include <thread>
#include <spdlog/spdlog.h>
#include "MessageHandler/MessageHandler.hpp"

namespace Command
{
    class Command
    {
        public:
            explicit Command(const Configuration::ConfigData &configuration, const CustomMessages &customMessages);
            void stopThread();
            void handle_message();
        private:
            zmq::context_t m_context{};
            zmq::socket_t m_sub;
            std::thread m_messageHandler;
            std::shared_ptr<MessageQueue> CommandQueue;
    };
}

#endif