#ifndef MESSAGE_TABLE_H_HL_MAI
#define MESSAGE_TABLE_H_HL_MAI

#include "message.h"

#include <HLMAI/database/table.h>

#include <Poco/Tuple.h>

#include <optional>

namespace database {
  
class MessageTable : public Table<Message>
{
 public:
  void Create() final;
  void SaveToMySQL(Message &message) final;

  std::optional<uint64_t> AddChatMessage(uint64_t chatId,
                                         uint64_t userId,
                                         std::string &messageText);
  std::vector<Message> GetChatMessages(uint64_t chatId);
 protected:
  MessageTable() = delete;
  MessageTable(const char *messageTableName);
  virtual ~MessageTable() = default;
};

}

#endif