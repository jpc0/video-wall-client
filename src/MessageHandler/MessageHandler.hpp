#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include <WnLSL/queues/ringbuffer_queue.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <optional>

class MessageHandler;

enum Destination
{
    VideoMessage = 1,
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
    int send(Message const &message){ return sendQueue->enqueue(message); };
    std::optional<Message> receive() { return receiveQueue->dequeue_noblock();};
private:
    std::optional<Message> queryMessage() { return sendQueue->dequeue_noblock(); };
    int pushMessage(Message const &message) { return receiveQueue->enqueue(message); };
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
    static void pushMessageUtil(Message const &message);
    inline static std::shared_ptr<MessageQueue> VideoQueue = std::make_shared<MessageQueue>();
    inline static std::shared_ptr<MessageQueue> DisplayQueue = std::make_shared<MessageQueue>();
    inline static std::shared_ptr<MessageQueue> CommandQueue = std::make_shared<MessageQueue>();
    inline static std::shared_ptr<MessageQueue> UnknownQueue = std::make_shared<MessageQueue>();

};


#endif