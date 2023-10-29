#ifndef CHAT_USER_TABLE_H_HL_MAI
#define CHAT_USER_TABLE_H_HL_MAI

#include "chat_user.h"

#include <HLMAI/singleton.h>
#include <HLMAI/database/table.h>

namespace database {

class ChatUserTable : public Table<ChatUser>,
                      public Singleton<ChatUserTable> {
 public:
  virtual void Create() override;
  virtual void SaveToMySQL(ChatUser &chatUser) override;

  void AddUserToChat(uint64_t chatId, uint64_t userId);
  std::vector<uint64_t> GetChatUsers(uint64_t chatId);
 private:
  friend Singleton<ChatUserTable>;

  ChatUserTable();
  ~ChatUserTable() = default;
};

}

#endif