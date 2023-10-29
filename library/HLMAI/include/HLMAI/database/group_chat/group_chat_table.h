#ifndef GROUP_CHAT_TABLE_H_HL_MAI
#define GROUP_CHAT_TABLE_H_HL_MAI

#include "group_chat.h"

#include <HLMAI/singleton.h>
#include <HLMAI/database/table.h>

#include <optional>

namespace database {

class GroupChatTable : public Table<GroupChat>,
                       public Singleton<GroupChatTable> {
 public:
  virtual void Create() override;
  virtual void SaveToMySQL(GroupChat &groupChat) override;

  std::optional<uint64_t> AddGroupChat(std::string &title); 
 private:
  friend Singleton<GroupChatTable>;

  GroupChatTable();
  ~GroupChatTable() = default;
};

}

#endif