#include "Messaging.hpp"


namespace Messaging {

Messaging::Messaging(const Configuration::ConfigData &configuration, const CustomMessages &customMessages)
 {
    m_sub = zmq::socket_t(m_context, zmq::socket_type::sub);
    m_sub.connect(configuration.zmq_server);
    m_sub.set(zmq::sockopt::subscribe, "");
    m_messageHandler = std::jthread{&Messaging::handle_message,
        customMessages.displaySingleImage,
        customMessages.displayDefaultImage
        };
    }

void Messaging::stopThread()
{
}

void Messaging::handle_message(uint32_t displaySingleImage, uint32_t displayDefaultImage)
{
    using json = nlohmann::json;
    zmq::message_t msg;
    auto res = m_sub.recv(msg, zmq::recv_flags::none);
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
                event.user.data1 = std::bit_cast<void *>(static_cast<std::string>(j["location"]).c_str());
                event.user.data2 = 0;
                SDL_PushEvent(&event);
            }
        }
        catch (nlohmann::detail::parse_error const &e)
        {
            std::cerr << "Error parsing json" << '\n';
            std::cerr << "Failed string was: " << msg.to_string() << '\n';
        }
    }
}}