#include "MessageHandler.hpp"

std::shared_ptr<MessageQueue> MessageHandler::registerReceiver(Destination destination)
{
    std::lock_guard<std::mutex> lock(rw_lock);
    auto is_destination = [destination](DestinationT i){return i.destination == destination;};
    auto result = std::find_if(begin(destinations), end(destinations), is_destination);
    if (result != std::end(destinations))
        return result->queue;

    std::shared_ptr<MessageQueue> messageQueue = std::make_shared<MessageQueue>();
    DestinationT dest{destination, messageQueue};
    destinations.push_back(dest);
    return messageQueue;
}

void MessageHandler::handleMessages()
{
    std::lock_guard<std::mutex> lock(rw_lock);
    for (auto destination : destinations)
    {
        if (auto message = destination.queue->queryMessage())
        {
            // TODO: This is ugly as sin, fixme probably also slow
            switch (message->messageDestination)
            {
            case Destination::VideoMessage:
                if ( auto dest = std::find_if(
                    begin(destinations), 
                    end(destinations), 
                    [](DestinationT i){return i.destination == Destination::VideoMessage;}
                    ); dest != std::end(destinations) 
                    || std::end(destinations)->destination == Destination::VideoMessage)
                    dest->queue->pushMessage(message.value());
                break;
            case Destination::DisplayMessage:
                if ( auto dest = std::find_if(
                    begin(destinations), 
                    end(destinations), 
                    [](DestinationT i){return i.destination == Destination::DisplayMessage;}
                    ); dest != std::end(destinations) 
                    || std::end(destinations)->destination == Destination::DisplayMessage)
                    dest->queue->pushMessage(message.value());
                break;
            case Destination::CommandMessage:
                if ( auto dest = std::find_if(
                    begin(destinations), 
                    end(destinations), 
                    [](DestinationT i){return i.destination == Destination::CommandMessage;}
                    ); dest != std::end(destinations) 
                    || std::end(destinations)->destination == Destination::CommandMessage)
                    dest->queue->pushMessage(message.value());
                break;
            case Destination::UnknownMessage:
                if ( auto dest = std::find_if(
                    begin(destinations), 
                    end(destinations), 
                    [](DestinationT i){return i.destination == Destination::UnknownMessage;}
                    ); dest != std::end(destinations) 
                    || std::end(destinations)->destination == Destination::UnknownMessage)
                    dest->queue->pushMessage(message.value());
                break;
            default:
                break;
            }
        }
    }
}