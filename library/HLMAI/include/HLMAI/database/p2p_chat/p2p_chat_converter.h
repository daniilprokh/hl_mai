#ifndef P2P_CHAT_CONVERTER_H_HL_MAI
#define P2P_CHAT_CONVERTER_H_HL_MAI

#include "p2p_chat.h"

#include <HLMAI/singleton.h>
#include <HLMAI/database/row_converter.h>

namespace database {

class P2pChatConverter : public RowConverter<P2pChatConverter, P2pChat>,
                         public Singleton<P2pChatConverter> 
{
  friend RowConverter<P2pChatConverter, P2pChat>;
  friend Singleton<P2pChatConverter>;

  P2pChatConverter() = default;
  ~P2pChatConverter() = default;

  void FromJSON(P2pChat &p2pChat,
                const Poco::JSON::Object::Ptr &jsonObject);
  void ToJSON(Poco::JSON::Object::Ptr &jsonObject,
              const P2pChat &p2pChat);
};

}

#endif