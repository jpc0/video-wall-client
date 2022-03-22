#include "Messaging.hpp"

Messaging::Messaging(const Configuration::ConfigData &configuration){
    m_sub = sub(m_context, zmq::socket_type::sub);
    m_sub.connect(configuration.zmq_server);
    m_sub.set(zmq::sockopt::subscribe, "");
    }


void Messaging::handle_message()
{
    zmq::message_t msg;
    auto res = m_sub.recv(msg, zmq::recv_flags::dontwait);
    if (res)
    {
        try
        {
            json j = json::parse(msg.to_string());
            if (j["command"].get<std::string>() == std::string("display_image"))
            {
                display.DisplaySingleImage(j["location"]);
            }
        }
        catch (nlohmann::detail::parse_error const &e)
        {
            std::cerr << "Error parsing json" << '\n';
            std::cerr << "Failed string was: " << msg.to_string() << '\n';
            continue;
        }
    }
}