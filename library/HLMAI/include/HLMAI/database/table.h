#ifndef TABLE_H_HL_MAI
#define TABLE_H_HL_MAI

#include "database.h"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

#include <vector>

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
          Poco::Data::Keywords::range(0, 1);
      select.execute();
      return result;
    }
  );
}

}

#endif