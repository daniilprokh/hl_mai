#include <HLMAI/database/user/user_table_shard.h>
#include <HLMAI/database/user/user_converter.h>
#include <HLMAI/database/user/user_feature.h>

#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>

#include <fstream>
#include <iostream>

using namespace Poco::Data::Keywords;

namespace database {

UserTableShard::UserTableShard() 
  : Table<User>("users", 6),
    Singleton<UserTableShard>()
{}

uint64_t UserTableShard::GetNextId(Poco::Data::Session &session) {
  uint64_t count = 0;
  auto &database = database::Database::GetInstance();
  for (std::string &hint : database.GetShardingHints()) {
    Poco::Data::Statement select(session);
    uint64_t shard_count;
    select << "SELECT COUNT(*) FROM %s %s",
      kName,
      hint,
      into(shard_count),
      now;

    count += shard_count;
  }

  return count + 1;
}

void UserTableShard::Insertion(Poco::Data::Session &session, User &user) {
  std::cout << 2 << std::endl;
  uint64_t next_id = GetNextId(session);
  std::cout << "next_id: " << next_id << std::endl;
  std::cout << 3 << std::endl;
  auto &database = database::Database::GetInstance();
  std::string hint = database.GetUserShardingHint(next_id);
  Poco::Data::Statement insert(session);
  insert << "INSERT INTO %s "
            "(user_id, first_name, last_name, email, login, password) "
            "VALUES(?, ?, ?, ?, ?, ?) %s",
      kName,
      hint,
      use(next_id),
      use(user.get<kUserFirstName>()),
      use(user.get<kUserLastName>()),
      use(user.get<kUserEmail>()),
      use(user.get<KUserLogin>()),
      use(user.get<kUserPassword>()),
      now;
  user.set<kUserId>(next_id);
}

void UserTableShard::Create() {
  SessionOperation<void>(
    [this](Poco::Data::Session& session) {
      auto &database = database::Database::GetInstance();
      for (std::string &hint : database.GetShardingHints()) {
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
                    << ") CHARACTER SET utf8 COLLATE utf8_unicode_ci; %s",
            this->kName,
            hint,
            now;
      }
    } 
  );
}

void UserTableShard::SaveToMySQL(User& user) {
  SessionOperation<void>(
    [this, &user](Poco::Data::Session& session) {
      std::cout << 1 << std::endl;
      this->Insertion(session, user);
    }
  );
}

std::optional<User> UserTableShard::ReadById(uint64_t id) {
  return SessionOperation<std::optional<User>>(
    [this, &id](Poco::Data::Session& session) {
      auto &database = database::Database::GetInstance();
      std::string hint = database.GetUserShardingHint(id);

      Poco::Data::Statement select(session);
      User user;
      select << "SELECT * "
                "FROM %s WHERE user_id = ? %s",
          this->kName,
          hint,
          into(user),
          use(id),
          now;
      Poco::Data::RecordSet rs(select);
      return rs.moveFirst() ? std::optional<User>(user)
                            : std::optional<User>();
    }
  );
}

std::optional<uint64_t> UserTableShard::GetUserId(std::string &login) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &login](Poco::Data::Session& session) {
      auto &database = database::Database::GetInstance();
      for (std::string &hint : database.GetShardingHints()) {
        Poco::Data::Statement select(session);
        uint64_t id;
        select << "SELECT user_id "
                  "FROM %s WHERE login = ? %s",
            this->kName,
            hint,
            into(id),
            use(login),
            now;

        Poco::Data::RecordSet rs(select);
        if (rs.moveFirst()) {
          return std::optional<uint64_t>(id);
        }
      }

      return std::optional<uint64_t>();
    }
  );
}

std::optional<uint64_t> UserTableShard::Authorize(std::string &login,
                                                  std::string &password) {
  return SessionOperation<std::optional<uint64_t>>(
    [this, &login, &password](Poco::Data::Session& session) {
      auto &database = database::Database::GetInstance();
      for (std::string &hint : database.GetShardingHints()) {
        Poco::Data::Statement select(session);
        uint64_t id;
        select << "SELECT user_id "
                  "FROM %s WHERE login = ? AND password = ? %s",
            this->kName,
            hint,
            into(id),
            use(login),
            use(password),
            now;

        Poco::Data::RecordSet rs(select);
        if(rs.moveFirst()) {
          return std::optional<uint64_t>(id);
        }
      }
      return std::optional<uint64_t>();
    }
  );
}

std::vector<User> UserTableShard::Search(std::string firstName,
                                         std::string lastName) {
  return SessionOperation<std::vector<User>>(
      [this, &firstName, &lastName](Poco::Data::Session& session) {
        auto &database = database::Database::GetInstance();
        std::vector<User> result;
        firstName += '%';
        lastName += '%';
        for (std::string &hint : database.GetShardingHints()) {
          Poco::Data::Statement select(session);
          select << "SELECT * "
                    "FROM %s "
                    "WHERE first_name LIKE ? AND last_name LIKE ? %s",
              this->kName,
              hint,
              into(result),
              use(firstName), use(lastName),
              now;
        }
        return result;
      }
  );
}

void UserTableShard::Preload(const std::string &filePath) {
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
