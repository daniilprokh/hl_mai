#ifndef DATABASE_H_HL_MAI
#define DATABASE_H_HL_MAI

#include <HLMAI/singleton.h>

#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionPool.h>

#include <functional>
#include <iostream>
#include <string>
#include <memory>

namespace database {

template <class SessionResult>
using HandleSession =
  std::function<SessionResult (Poco::Data::Session&)>;

class Database : public Singleton<Database>  {
  public:
    template <class SessionResult>
    SessionResult SessionLife(
      const HandleSession<SessionResult>& handleSession,
      std::ostream& errorOutput = std::cout);

    std::vector<std::string> GetShardingHints();
    std::string GetUserShardingHint(uint64_t userId);
  private:
    friend Singleton<Database>;

    Database();
    ~Database() = default;

    std::string FormShardingName(uint8_t nodeIdx);

    const uint8_t kNodeCount{2};

    std::string connection_str_;
    std::unique_ptr<Poco::Data::SessionPool> pool_;
};

template <class SessionResult>
SessionResult Database::SessionLife(
    const HandleSession<SessionResult>& handleSession,
    std::ostream& errorOutput) {
  try {
    Poco::Data::Session session = Poco::Data::Session(pool_->get());
    return handleSession(session);
  }
  catch (Poco::Data::MySQL::ConnectionException &e)
  {
    errorOutput << "connection: " << e.what() << std::endl;
    throw;
  }
  catch (Poco::Data::MySQL::StatementException &e)
  {
    errorOutput << "statement: " << e.what() << std::endl;
    throw;
  }
}

#endif

}
