#include "Command.hpp"
#include <filesystem>
#include "cpr/cpr.h"

namespace Command
{

    Command::Command(const Configuration::ConfigData &configuration)
          : CommandQueue{MessageHandler::registerReceiver(Destination::CommandMessage)}
    {
        m_sub = zmq::socket_t(m_context, zmq::socket_type::sub);
        m_sub.connect(configuration.zmq_server);
        m_sub.set(zmq::sockopt::subscribe, "");
        m_messageHandler = std::thread{&Command::handle_message,
                                        this,
                                        };
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

    std::vector<std::string> split(std::string const &str, std::string const &splitBy)
    {
      std::vector<std::string> tokens{};
      /* Store the original string in the array, so we can loop the rest
     * of the algorithm. */
      tokens.push_back(str);

      // Store the size of what we're splicing out.
      size_t splitLen = splitBy.size();
      // Create a string for temporarily storing the fragment we're processing.
      std::string frag;
      // Loop infinitely - break is internal.
      while(true)
      {
        /* Store the last string in the vector, which is the only logical
         * candidate for processing. */
        frag = tokens.back();
        /* The index where the split is. */
        auto splitAt = frag.find(splitBy);
        // If we didn't find a new split point...
        if(splitAt == std::string::npos)
        {
          // Break the loop and (implicitly) return.
          break;
        }
        /* Put everything from the left side of the split where the string
         * being processed used to be. */
        tokens.back() = frag.substr(0, splitAt);
        /* Push everything from the right side of the split to the next empty
         * index in the vector. */
        tokens.push_back(frag.substr(splitAt+splitLen, frag.size()-(splitAt+splitLen)));
      }
      return tokens;
    }

    void Command::handle_message()
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
                        std::string location = j["location"];
                        spdlog::info("{}", location);
                        auto seglist = split(location, "/");
                        auto file_name = seglist.back();
                        std::string local_path = "/tmp/videowall/" + file_name;
                        spdlog::info("{}", local_path);
                        spdlog::info("{}", local_path.c_str());
                        if (!std::filesystem::exists("/tmp/videowall"))
                          std::filesystem::create_directory("/tmp/videowall");
                        if (!std::filesystem::exists(local_path))
                        {
                          cpr::Response r = cpr::Get(cpr::Url{location});
                          std::fstream s{local_path, std::fstream::binary | std::fstream::trunc | std::fstream::out};
                          s.write(r.text.data(), r.downloaded_bytes);
                        }
                        CommandQueue->send({
                          Destination::DisplayMessage,
                          {{"Command", "ImagePath"}, {"Data", local_path}}
                        });
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
