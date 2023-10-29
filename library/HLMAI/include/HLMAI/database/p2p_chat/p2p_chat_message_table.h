#ifndef P2P_CHAT_MESSAGE_TABLE_H_HL_MAI
#define P2P_CHAT_MESSAGE_TABLE_H_HL_MAI

#include <HLMAI/singleton.h>
#include <HLMAI/database/message/message_table.h>

namespace database {

class P2pChatMessageTable : public MessageTable,
                            public Singleton<P2pChatMessageTable> 
{
  friend Singleton<P2pChatMessageTable>;

  P2pChatMessageTable() 
    : MessageTable("p2p_chat_messages"),
      Singleton<P2pChatMessageTable>()
  {}

  ~P2pChatMessageTable() = default;
};

}

#endif