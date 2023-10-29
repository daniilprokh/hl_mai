#include <HLMAI/database/chat_user/chat_user_converter.h>

#include <HLMAI/database/chat_user/chat_user_feature.h>

namespace database {

void ChatUserConverter::FromJSON(ChatUser &chatUser,
                                 const Poco::JSON::Object::Ptr &jsonObject) {
  chatUser.set<kChatUserId>(
      jsonObject->getValue<uint64_t>("user_id"));
  chatUser.set<kChatUserChatId>(
      jsonObject->getValue<uint64_t>("chat_id"));
}

void ChatUserConverter::ToJSON(const ChatUser &chatUser,
                               Poco::JSON::Object::Ptr &jsonObject) {
  jsonObject->set("user_id", chatUser.get<kChatUserId>());
  jsonObject->set("chat_id", chatUser.get<kChatUserChatId>());
}

}