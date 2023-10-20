#ifndef MESSAGE_TABLE_H_HL_MAI
#define MESSAGE_TABLE_H_HL_MAI

#include "message.h"

#include <HLMAI/database/table.h>

#include <Poco/Tuple.h>

namespace database {
  
class MessageTable : public Table<Message>, public Singleton<MessageTable> {
public:


  void Create() override;
  void SaveToMySQL(Message& message) override;

  std::vector<Message> GetChatMessages(uint64_t chatId);
  private:
    friend Singleton<MessageTable>;

    MessageTable();
    ~MessageTable() = default;

};

}

#endif