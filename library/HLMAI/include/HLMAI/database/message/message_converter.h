#ifndef MESSAGE_CONVERTER_H_HL_MAI
#define MESSAGE_CONVERTER_H_HL_MAI

#include "message.h"
#include "message_feature.h"

#include <HLMAI/database/row_converter.h>
#include <HLMAI/singleton.h>

namespace database {

class MessageConverter : public RowConverter<MessageConverter, Message>,
                         public Singleton<MessageConverter> 
{
  friend RowConverter<MessageConverter, Message>;
  friend Singleton<MessageConverter>;

  MessageConverter() = default;
  ~MessageConverter() = default;

  void FromJSON(Message &message,
                const Poco::JSON::Object::Ptr &jsonObject);
  void ToJSON(Poco::JSON::Object::Ptr &jsonObject,
              const Message &message);
};

}

#endif
