#include "Command.hpp"

namespace Command
{

    Command::Command(const Configuration::ConfigData &configuration, const CustomMessages &customMessages)
    {
        m_sub = zmq::socket_t(m_context, zmq::socket_type::sub);
        m_sub.connect(configuration.zmq_server);
        m_sub.set(zmq::sockopt::subscribe, "");
        m_messageHandler = std::thread{&Command::handle_message,
                                        this,
                                        customMessages.displaySingleImage,
                                        customMessages.displayDefaultImage};
        m_messageHandler.detach();
    }

    void Command::stopThread()
    {
        // FIXME: WE need to implement a better way to exit the context, maybe
        // look at a zmq poller
        try
        {
            m_context.shutdown();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    
    void Command::handle_message(uint32_t displaySingleImage, [[ maybe_unused ]] uint32_t displayDefaultImage)
    {
        while (true) {
            using json = nlohmann::json;
            zmq::message_t msg;
            zmq::recv_result_t res;
            // FIXME: WE need to implement a better way to exit the context, maybe
            // look at a zmq poller
            try 
            {
                res = m_sub.recv(msg, zmq::recv_flags::none);
            }
            catch(const zmq::error_t &e)
            {
                spdlog::info(e.what());
                break;
            }
            if (res)
            {
                try
                {
                    json j = json::parse(msg.to_string());
                    if (j["command"].get<std::string>() == std::string("display_image"))
                    {
                        SDL_Event event;
                        SDL_memset(&event, 0, sizeof(event));
                        event.type = displaySingleImage;
                        event.user.code = 1;
                        event.user.data1 = const_cast<void *>(reinterpret_cast<void const *>(static_cast<std::string>(j["location"]).c_str()));
                        event.user.data2 = 0;
                        SDL_PushEvent(&event);
                    }
                    continue;
                }
                catch (nlohmann::detail::parse_error const &e)
                {
                    spdlog::info("Failed to parse json string: {}", msg.to_string());
                }
            }
            break;
        } 
    }
}
