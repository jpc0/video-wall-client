#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include <WnLSL/queues/ringbuffer_queue.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <optional>

class MessageHandler;

enum Destination
{
    NoneMessage = 0,
    VideoMessage,
    DisplayMessage,
    CommandMessage,
    UnknownMessage
};


struct Message
{
    Destination messageDestination;
    nlohmann::json message;
};

class MessageQueue
{
public:
    int send(Message message){ return sendQueue->enqueue(message); };
    std::optional<Message> receive() { return receiveQueue->dequeue_noblock();};
private:
    std::optional<Message> queryMessage() { return sendQueue->dequeue_noblock(); };
    int pushMessage(Message message) { return receiveQueue->enqueue(message); };
    using queue = WnLSL::blocking_rb_queue<Message>;
    std::unique_ptr<queue>  sendQueue = std::make_unique<queue>();
    std::unique_ptr<queue> receiveQueue  = std::make_unique<queue>();

    friend MessageHandler;
};

class MessageHandler
{
public:
    static std::shared_ptr<MessageQueue> registerReceiver(Destination destination);
    static void handleMessages();
private:
    struct DestinationT
    {
        Destination destination;
        std::shared_ptr<MessageQueue> queue;
    };
    inline static std::mutex rw_lock{};
    inline static std::vector<DestinationT> destinations{};
    // destinations should probably be some sort of map not a vec
};


#endif