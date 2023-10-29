#include <HLMAI/database/message/message_converter.h>

#include <HLMAI/database/message/message_feature.h>

namespace database {

void MessageConverter::FromJSON(Message& message,
                                const Poco::JSON::Object::Ptr& jsonObject) {
  message.set<kMessageId>(
      jsonObject->getValue<uint64_t>("message_id"));
  message.set<kMessageChatId>(
      jsonObject->getValue<uint64_t>("chat_id"));
  message.set<kMessageUserId>(
      jsonObject->getValue<uint64_t>("user_id"));
  message.set<kMessageText>(
      jsonObject->getValue<std::string>("text"));
  message.set<kMessageDateTime>(
      jsonObject->getValue<Poco::DateTime>("date_time"));
}

void MessageConverter::ToJSON(Poco::JSON::Object::Ptr& jsonObject,
                              const Message& message) {
  jsonObject->set("message_id", message.get<kMessageId>());
  jsonObject->set("chat_id",    message.get<kMessageChatId>());
  jsonObject->set("user_id",    message.get<kMessageUserId>());
  jsonObject->set("text",       message.get<kMessageText>());
  jsonObject->set("date_time",  message.get<kMessageDateTime>());
}

}
