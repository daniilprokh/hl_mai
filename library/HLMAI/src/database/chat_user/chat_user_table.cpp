#include <HLMAI/database/chat_user/chat_user_table.h>

#include <HLMAI/database/chat_user/chat_user_feature.h>

using namespace Poco::Data::Keywords;

namespace database {

ChatUserTable::ChatUserTable()
  : Table<ChatUser>("chat_users", 2),
    Singleton<ChatUserTable>()
{}

void ChatUserTable::Create() {
  SessionOperation<void>(
    [this](Poco::Data::Session& session){
      Poco::Data::Statement create_stmt(session);
      create_stmt << "CREATE TABLE IF NOT EXISTS %s ("
                     "user_id INT NOT NULL,"
                     "chat_id INT NOT NULL);",
          this->kName,
          now;
    }
  );
}

void ChatUserTable::SaveToMySQL(ChatUser &chatUser) {
  SessionOperation<void>(
    [this, &chatUser](Poco::Data::Session &session) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO %s "
                "(user_id, chat_id) "
                "VALUES(?, ?)",
          this->kName,
          use(chatUser.get<kChatUserId>()),
          use(chatUser.get<kChatUserChatId>()),
          now;
    }
  );
}

std::vector<uint64_t> ChatUserTable::GetChatUsers(uint64_t chatId) {
  return SessionOperation<std::vector<uint64_t>>(
    [this, &chatId](Poco::Data::Session &session) {
      Poco::Data::Statement select(session);
      std::vector<uint64_t> user_ids;
      uint64_t user_id;
      select << "SELECT user_id FROM %s "
                "WHERE chat_id = ?",
          this->kName,
          into(user_id),
          use(chatId),
          range(0, 1);
      
      while (!select.done()) {
        select.execute();
        user_ids.push_back(user_id);
      }

      return user_ids;
    }
  );
}

}