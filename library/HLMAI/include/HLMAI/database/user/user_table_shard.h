#ifndef USER_TABLE_H_HL_MAI
#define USER_TABLE_H_HL_MAI

#include "user.h"

#include <HLMAI/database/table.h>

#include <optional>

namespace database {

class UserTableShard : public Table<User>,
                       public Singleton<UserTableShard>
{
 public:
  void Create() override;
  void SaveToMySQL(User& user) override;

  std::optional<User> ReadById(uint64_t id);
  std::optional<uint64_t> GetUserId(std::string &login);
  std::optional<uint64_t> Authorize(std::string &login,
                                    std::string &password);
  std::vector<User> Search(std::string firstName,
                           std::string lastName);

  void Preload(const std::string &filePath);
 private:
  friend Singleton<UserTableShard>;

  UserTableShard();
  ~UserTableShard() = default;

  uint64_t GetNextId(Poco::Data::Session &session);

  void Insertion(Poco::Data::Session &session, User &user);
};

}
#endif