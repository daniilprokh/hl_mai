#include <HLMAI/database/p2p_chat/p2p_chat_table.h>

#include <HLMAI/database/p2p_chat/p2p_chat_feature.h>

#include <Poco/Data/RecordSet.h>

using namespace Poco::Data::Keywords;

namespace database {

P2pChatTable::P2pChatTable()
  : Table<P2pChat>("p2p_chats", 3),
    Singleton<P2pChatTable>()
{}

void P2pChatTable::Create() {
  SessionOperation<void>(
    [this](Poco::Data::Session& session){
      Poco::Data::Statement create_stmt(session);
      create_stmt << "CREATE TABLE IF NOT EXISTS %s ("
                  << "PRIMARY KEY (p2p_chat_id),"
                  << "p2p_chat_id INT NOT NULL AUTO_INCREMENT,"
                  << "user_id1    INT NOT NULL,"
                  << "user_id2    INT NOT NULL);",
          this->kName,
          now;
    }
  );
}

void P2pChatTable::SaveToMySQL(P2pChat &p2pChat) {
  SessionOperation<void>(
    [this, &p2pChat](Poco::Data::Session &session) {
      Poco::Data::Statement insert(session);
      insert << "INSERT INTO %s "
                "(user_id1, user_id2) "
                "VALUES(?, ?)",
          this->kName,
          use(p2pChat.get<kP2pChatUserId1>()),
          use(p2pChat.get<kP2pChatUserId2>()),
          now;
    }
  );
}

std::optional<uint64_t> P2pChatTable::GetP2pChatId(uint64_t userId1,
                                                   uint64_t userId2) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &userId1, &userId2](Poco::Data::Session &session) {
      Poco::Data::Statement select(session);
      uint64_t id;
      select << "SELECT p2p_chat_id FROM %s "
                "WHERE (user_id1 = %[0]d AND user_id2 = %[1]d) OR "
                      "(user_id1 = %[1]d AND user_id2 = %[0]d)",
          into(id),
          this->kName,
          userId1, userId2,
          now;

      Poco::Data::RecordSet rs(select);
      return rs.moveFirst() ? std::optional<uint64_t>(id)
                            : std::nullopt;
    }
  );
}

}