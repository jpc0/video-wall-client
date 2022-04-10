#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include <WnLSL/queues/ringbuffer_queue.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <optional>

class MessageHandler;

enum Destination
{
    None = 0,
    Video,
    Display,
    Command,
    Unknown
};

struct Message
{
    Destination messageDestination;
    nlohmann::json message;
};

class MessageQueue
{
public:
    int send();
    std::optional<Message> receive();
private:
    Message QueryMessage();
    WnLSL::blocking_rb_queue<Message> sendQueue{};
    WnLSL::blocking_rb_queue<Message> receiveQueue{};

    friend MessageHandler;
};

class MessageHandler
{
public:
    MessageQueue* registerReceiver();
private:
    struct DestinationT
    {
        Destination destination;
        MessageQueue queue;
    };
    std::vector<DestinationT> destinations{};
};


#endif