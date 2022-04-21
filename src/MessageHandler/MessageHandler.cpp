#include "MessageHandler.hpp"

std::shared_ptr<MessageQueue> MessageHandler::registerReceiver(Destination destination)
{
    switch (destination)
    {
    case Destination::CommandMessage:
      return CommandQueue;
      break;
    case Destination::VideoMessage:
      return VideoQueue;
      break;
    case Destination::DisplayMessage:
      return DisplayQueue;
      break;
    case Destination::UnknownMessage:
      return UnknownQueue;
      break;
    default:
      return nullptr;
    }
}

void MessageHandler::pushMessageUtil(Message const &message)
{
  switch (message.messageDestination)
  {
  case Destination::DisplayMessage:
    DisplayQueue->pushMessage(message);
    break;
  case Destination::CommandMessage:
    CommandQueue->pushMessage(message);
    break;
  case Destination::VideoMessage:
    VideoQueue->pushMessage(message);
    break;
  case Destination::UnknownMessage:
    UnknownQueue->pushMessage(message);
    break;
  default:
    break;
  }
}

void MessageHandler::handleMessages()
{
  if (auto message = VideoQueue->queryMessage())
    pushMessageUtil(message.value());
  if (auto message = CommandQueue->queryMessage())
    pushMessageUtil(message.value());
  if (auto message = DisplayQueue->queryMessage())
    pushMessageUtil(message.value());
  if (auto message = UnknownQueue->queryMessage())
    pushMessageUtil(message.value());
}