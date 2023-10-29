#ifndef USER_TABLE_H_HL_MAI
#define USER_TABLE_H_HL_MAI

#include "user.h"

#include <HLMAI/database/table.h>

#include <optional>

namespace database {

class UserTable : public Table<User>,
                  public Singleton<UserTable>
{
 public:
  void Create() override;
  void SaveToMySQL(User& user) override;

  std::optional<User> ReadById(uint64_t id);
  std::optional<uint64_t> GetUserId(std::string login);
  std::optional<uint64_t> Authorize(std::string login,
                                    std::string password);
  std::vector<User> Search(std::string firstName,
                            std::string lastName);

  void Preload(const std::string &filePath);
 private:
  friend Singleton<UserTable>;

  UserTable();
  ~UserTable() = default;

  void Insertion(Poco::Data::Session &session, User &user);
};

}
#endif