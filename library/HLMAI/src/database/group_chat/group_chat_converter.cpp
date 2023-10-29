#include <HLMAI/database/group_chat/group_chat_converter.h>

#include <HLMAI/database/group_chat/group_chat_feature.h>

namespace database {

void GroupChatConverter::FromJSON(GroupChat &groupChat,
                                  const Poco::JSON::Object::Ptr &jsonObject) {
  groupChat.set<kGroupChatId>(
      jsonObject->getValue<uint64_t>("group_chat_id"));
  groupChat.set<kGroupChatTitle>(
      jsonObject->getValue<std::string>("title"));
}

void GroupChatConverter::ToJSON(const GroupChat &groupChat,
                               Poco::JSON::Object::Ptr &jsonObject) {
  jsonObject->set("group_chat_id", groupChat.get<kGroupChatId>());
  jsonObject->set("title",         groupChat.get<kGroupChatTitle>());
}
  
}