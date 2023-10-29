#include <HLMAI/database/group_chat/group_chat_table.h>

#include <HLMAI/database/group_chat/group_chat_feature.h>

using namespace Poco::Data::Keywords;

namespace database {

GroupChatTable::GroupChatTable()
  : Table<GroupChat>("group_chats", 2),
    Singleton<GroupChatTable>()
{}

void GroupChatTable::Create() {
  SessionOperation<void>(
    [this](Poco::Data::Session& session){
      Poco::Data::Statement create_stmt(session);
      create_stmt << "CREATE TABLE IF NOT EXISTS %s ("
                     "PRIMARY KEY (group_chat_id),"
                     "group_chat_id INT          NOT NULL AUTO_INCREMENT,"
                     "title         VARCHAR(256) NOT NULL);",
          this->kName,
          now;
    }
  );
}

void GroupChatTable::SaveToMySQL(GroupChat &groupChat) {
  SessionOperation<void>(
    [this, &groupChat](Poco::Data::Session &session) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO %s "
                "(title) "
                "VALUES(?)",
          this->kName,
          use(groupChat.get<kGroupChatTitle>()),
          now;
    }
  );
}

std::optional<uint64_t> GroupChatTable::AddGroupChat(std::string &title) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &title](Poco::Data::Session &session) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO %s "
                "(title) "
                "VALUES(?)",
          this->kName,
          use(title),
          now;

      return this->GetLastId(session);
    }
  );
}

}