#ifndef GROUP_CHAT_MESSAGE_TABLE_H_HL_MAI
#define GROUP_CHAT_MESSAGE_TABLE_H_HL_MAI

#include <HLMAI/singleton.h>
#include <HLMAI/database/message/message_table.h>

namespace database {

class GroupChatMessageTable : public MessageTable,
                              public Singleton<GroupChatMessageTable> 
{
  friend Singleton<GroupChatMessageTable>;

  GroupChatMessageTable() 
    : MessageTable("group_chat_messages"),
      Singleton<GroupChatMessageTable>()
  {}

  ~GroupChatMessageTable() = default;
};

}

#endif