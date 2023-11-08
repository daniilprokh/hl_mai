#ifndef TABLE_H_HL_MAI
#define TABLE_H_HL_MAI

#include "database.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

#include <vector>

#include <optional>

namespace database {

template <class Tuple>
class Table {
 public:
  std::vector<Tuple> ReadAll();

  virtual void Create() = 0;
  virtual void SaveToMySQL(Tuple& tuple) = 0;
 protected:
  Table() = delete;
  Table(const char* name, size_t columnCount);

  virtual ~Table() = default;

  std::optional<uint64_t> GetLastId(Poco::Data::Session &session);

  template <class SessionResult>
  SessionResult SessionOperation(HandleSession<SessionResult> handleSession);

  const std::string kName;
  const size_t kColumnCount;
};

template <class Tuple>
Table<Tuple>::Table(const char* name, size_t columnCount) 
  : kName(name), kColumnCount(columnCount)
{}

template <class Tuple>
std::optional<uint64_t> Table<Tuple>::GetLastId(
    Poco::Data::Session &session) {
  uint64_t id;
  Poco::Data::Statement select(session);
  select << "SELECT LAST_INSERT_ID()", 
      Poco::Data::Keywords::into(id),
      Poco::Data::Keywords::range(0, 1);

  if (!select.done())
  {
    select.execute();
  }
    
  Poco::Data::RecordSet rs(select);
  return rs.moveFirst() ? std::optional<uint64_t>(id)
                        : std::nullopt;
}

template <class Tuple>
template <class SessionResult>
SessionResult Table<Tuple>::SessionOperation(
    HandleSession<SessionResult> handleSession) {
  Database& database = Database::GetInstance();
  return database.SessionLife(handleSession);
}

template <class Tuple>
std::vector<Tuple> Table<Tuple>::ReadAll() {
  return SessionOperation<std::vector<Tuple>>(
    [this](const Poco::Data::Session& session) {
      Poco::Data::Statement select(session);
      std::vector<Tuple> result;
      select << "SELECT * FROM %s",
          this->kName,
          Poco::Data::Keywords::into(result),
          Poco::Data::Keywords::range(0, 1),
          Poco::Data::Keywords::now;
      return result;
    }
  );
}

}

#endif