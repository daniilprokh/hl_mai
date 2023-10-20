#ifndef MESSAGE_CONVERTER_H_HL_MAI
#define MESSAGE_CONVERTER_H_HL_MAI

#include "message.h"
#include "message_feature.h"

#include <HLMAI/database/row_converter.h>
#include <HLMAI/singleton.h>

namespace database {

class MessageConverter : public RowConverter<MessageConverter, Message>,
                         public Singleton<MessageConverter> {
  private:
    friend RowConverter<MessageConverter, Message>;
    friend Singleton<MessageConverter>;

    MessageConverter() = default;
    ~MessageConverter() = default;

    void FromJSON(Message& message,
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
          jsonObject->getValue<std::string>("date_time"));
    }
    void ToJSON(Poco::JSON::Object::Ptr& jsonObject,
                const Message& message) {
      jsonObject->set("message_id", message.get<kMessageId>());
      jsonObject->set("chat_id",    message.get<kMessageChatId>());
      jsonObject->set("user_id",    message.get<kMessageUserId>());
      jsonObject->set("text",       message.get<kMessageText>());
      jsonObject->set("date_time",  message.get<kMessageDateTime>());
    }
};

}

#endif
