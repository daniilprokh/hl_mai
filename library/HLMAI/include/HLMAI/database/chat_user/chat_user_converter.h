#ifndef CHAT_USER_CONVERTER_H_HL_MAI
#define CHAT_USER_CONVERTER_H_HL_MAI

#include "chat_user.h"

#include <HLMAI/singleton.h>
#include <HLMAI/database/row_converter.h>

namespace database {

class ChatUserConverter : public RowConverter<ChatUserConverter, ChatUser>,
                          public Singleton<ChatUserConverter> {
  friend RowConverter<ChatUserConverter, ChatUser>;
  friend Singleton<ChatUserConverter>;

  void FromJSON(ChatUser &chatUser,
                const Poco::JSON::Object::Ptr &jsonObject);
  void ToJSON(const ChatUser &chatUser,
              Poco::JSON::Object::Ptr &jsonObject);
};

}


#endif