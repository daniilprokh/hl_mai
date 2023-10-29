#include <HLMAI/database/message/message_table.h>
#include <HLMAI/database/message/message_feature.h>

#include <Poco/Data/RecordSet.h>

using namespace Poco::Data::Keywords;

namespace database
{

MessageTable::MessageTable(const char *messageTableName) 
  : Table<Message>(messageTableName, 5)
{}

void MessageTable::Create() {
  SessionOperation<void>(
    [this](Poco::Data::Session& session){
      Poco::Data::Statement create_stmt(session);
      create_stmt << "CREATE TABLE IF NOT EXISTS %s ("
                  << "PRIMARY KEY (message_id),"
                  << "message_id INT           NOT NULL AUTO_INCREMENT,"
                  << "chat_id    INT           NOT NULL,"
                  << "user_id    INT           NOT NULL,"
                  << "text       VARCHAR(1024) NULL,"
                  << "date_time  DATETIME      DEFAULT NOW());",
          this->kName,
          now;
    }
  );
}

void MessageTable::SaveToMySQL(Message &message) {
  SessionOperation<void>(
    [this, &message](Poco::Data::Session &session) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO %s "
                "(chat_id,user_id,text,date_time) "
                "VALUES(?, ?, ?, ?, ?)",
          this->kName,
          use(message.get<kMessageChatId>()),
          use(message.get<kMessageUserId>()),
          use(message.get<kMessageText>()),
          use(message.get<kMessageDateTime>()),
          now;
    }
  );
}

std::optional<uint64_t> MessageTable::AddChatMessage(
    uint64_t chatId,
    uint64_t userId,
    std::string &messageText) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &chatId, &userId, &messageText](Poco::Data::Session& session) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO %s "
                "(chat_id,user_id,text) "
                "VALUES(?, ?, ?)",
          this->kName,
          use(chatId), use(userId), use(messageText),
          now;

      return this->GetLastId(session);
    }
  );
}

std::vector<Message> MessageTable::GetChatMessages(uint64_t chatId) {
  return SessionOperation<std::vector<Message>>(
    [this, &chatId](Poco::Data::Session &session) {
      Poco::Data::Statement select(session);
      std::vector<Message> result;
      select << "SELECT *"
             << "FROM %s WHERE chat_id = ?",
          this->kName,
          into(result),
          use(chatId),
          range(0, 1),
          now;
      return result;
    }
  );        
}

}
