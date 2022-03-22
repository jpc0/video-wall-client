#include "Messaging.hpp"


namespace Messaging {

Messaging::Messaging(const Configuration::ConfigData &configuration) {
    m_sub = zmq::socket_t(m_context, zmq::socket_type::sub);
    m_sub.connect(configuration.zmq_server);
    m_sub.set(zmq::sockopt::subscribe, "");
    displaySingleImage = SDL_RegisterEvents(m_NUM_EVENTS);
    displayDefaultImage = displaySingleImage +1;
    playVideo = displaySingleImage +2;

    }



void Messaging::handle_message()
{

using json = nlohmann::json;
    zmq::message_t msg;
    auto res = m_sub.recv(msg, zmq::recv_flags::dontwait);
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