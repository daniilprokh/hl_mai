#include <HLMAI/database/p2p_chat/p2p_chat_converter.h>

#include <HLMAI/database/p2p_chat/p2p_chat_feature.h>

namespace database {

void P2pChatConverter::FromJSON(P2pChat &p2pChat,
                                const Poco::JSON::Object::Ptr &jsonObject) {
  p2pChat.set<kP2pChatId>(
      jsonObject->getValue<uint64_t>("p2p_chat_id"));
  p2pChat.set<kP2pChatUserId1>(
      jsonObject->getValue<uint64_t>("user_id1"));
  p2pChat.set<kP2pChatUserId2>(
      jsonObject->getValue<uint64_t>("user_id2"));
}

void P2pChatConverter::ToJSON(Poco::JSON::Object::Ptr &jsonObject,
                              const P2pChat &p2pChat) {
  jsonObject->set("p2p_chat_id",  p2pChat.get<kP2pChatId>());
  jsonObject->set("user_id1",     p2pChat.get<kP2pChatUserId1>());
  jsonObject->set("user_id2",     p2pChat.get<kP2pChatUserId2>());
}

}

