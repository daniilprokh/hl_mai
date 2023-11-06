#include <HLMAI/database/user/user_table.h>
#include <HLMAI/database/user/user_converter.h>
#include <HLMAI/database/user/user_feature.h>

#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>

#include <fstream>

using namespace Poco::Data::Keywords;

namespace database {

UserTable::UserTable() 
  : Table<User>("users", 6),
    Singleton<UserTable>()
{}

void UserTable::Insertion(Poco::Data::Session &session, User &user) {
  Poco::Data::Statement insert(session);
  insert << "INSERT INTO %s "
            "(first_name,last_name,email,login,password) "
            "VALUES(?, ?, ?, ?, ?)",
      kName,
      use(user.get<kUserFirstName>()),
      use(user.get<kUserLastName>()),
      use(user.get<kUserEmail>()),
      use(user.get<KUserLogin>()),
      use(user.get<kUserPassword>()),
      now;
}

void UserTable::Create() {
  SessionOperation<void>(
    [this](Poco::Data::Session& session) {
      Poco::Data::Statement create_stmt(session);
      create_stmt << "CREATE TABLE IF NOT EXISTS %s ("
                  << "PRIMARY KEY (user_id),"
                  << "user_id    INT          NOT NULL AUTO_INCREMENT,"
                  << "first_name VARCHAR(256) NOT NULL,"
                  << "last_name  VARCHAR(256) NOT NULL,"
                  << "email      VARCHAR(256) NOT NULL,"
                  << "login      VARCHAR(256) NOT NULL,"
                  << "password   VARCHAR(256) NOT NULL,"
                  << "  INDEX name_index  USING BTREE (first_name, last_name),"
                  << "  INDEX email_index USING HASH  (email)"
                  << ") CHARACTER SET utf8 COLLATE utf8_unicode_ci;",
          this->kName,
          now;
    }
  );
}

void UserTable::SaveToMySQL(User& user) {
  SessionOperation<void>(
    [this, &user](Poco::Data::Session& session) {
      this->Insertion(session, user);

      if (auto id = GetLastId(session)) {
        user.set<kUserId>(id.value());
      }
    }
  );
}

std::optional<User> UserTable::ReadById(uint64_t id) {
  return SessionOperation<std::optional<User>>(
    [this, &id](Poco::Data::Session& session) {
      Poco::Data::Statement select(session);
      User user;
      select << "SELECT * "
                "FROM %s WHERE user_id = ?",
          this->kName,
          into(user),
          use(id),
          now;
      Poco::Data::RecordSet rs(select);
      return rs.moveFirst() ? std::optional<User>(user)
                            : std::optional<User>();
    }
  );
}

std::optional<uint64_t> UserTable::GetUserId(std::string login) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &login](Poco::Data::Session& session) {
      Poco::Data::Statement select(session);
      uint64_t id;
      select << "SELECT user_id "
                "FROM %s WHERE login = ?",
          this->kName,
          into(id),
          use(login),
          now;

      Poco::Data::RecordSet rs(select);
      return rs.moveFirst() ? std::optional<uint64_t>(id)
                            : std::nullopt;
    }
  );
}

std::optional<uint64_t> UserTable::Authorize(std::string login,
                                             std::string password) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &login, &password](Poco::Data::Session& session) {
      Poco::Data::Statement select(session);
      uint64_t id;
      select << "SELECT user_id "
                "FROM %s WHERE login = ? AND password = ?",
          this->kName,
          into(id),
          use(login),
          use(password),
          now;

      Poco::Data::RecordSet rs(select);
      return rs.moveFirst() ? std::optional<uint64_t>(id)
                            : std::nullopt;
    }
  );
}

std::vector<User> UserTable::Search(std::string firstName,
                                    std::string lastName) {
  return SessionOperation<std::vector<User>>(
      [this, &firstName, &lastName](Poco::Data::Session& session) {
        Poco::Data::Statement select(session);
        std::vector<User> result;
        User user;
        firstName += '%';
        lastName += '%';
        select << "SELECT * "
                  "FROM %s "
                  "WHERE first_name LIKE ? AND last_name LIKE ?",
            this->kName,
            into(result),
            use(firstName), use(lastName),
            now;
        return result;
      }
  );
}

void UserTable::Preload(const std::string &filePath) {
  std::ifstream input(filePath);
  if (!input.is_open()) {
    return;
  }
  std::string json_str(std::istream_iterator<char>(input), {});
  input.close();

  Poco::JSON::Parser parser;
  Poco::Dynamic::Var result = parser.parse(json_str);
  Poco::JSON::Array::Ptr arr = result.extract<Poco::JSON::Array::Ptr>();

  std::vector<User> users;
  auto &user_converter = UserConverter::GetInstance();
  for (size_t idx = 0; idx < arr->size(); idx += 1) {
    Poco::JSON::Object::Ptr object = arr->getObject(idx);
     users.push_back(user_converter.RowFromJSON(object));
  }

  SessionOperation<void>(
    [this, &users](Poco::Data::Session& session) {
      for (User &user : users) {
        this->Insertion(session, user);
      }
    }
  );
}

}
