#include <HLMAI/database/user/user_converter.h>

#include <HLMAI/cache.h>

#include <Poco/JSON/Parser.h>

#include <cppkafka/cppkafka.h>

#include <sstream>

namespace database {

void UserConverter::UserToCache(const User &user) {
  std::stringstream ss;
  Poco::JSON::Stringifier::stringify(RowToJSON(user), ss);
  std::string message = ss.str();
  Cache::GetInstance().Put(user.get<kUserId>(), message);
}

std::optional<User> UserConverter::UserFromCache(uint64_t userId) {
  std::string result;
  if (Cache::GetInstance().Get(userId, result)) {
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(result);
    return RowFromJSON(result.extract<Poco::JSON::Object::Ptr>());
  }

  return std::nullopt;
}

void UserConverter::SendToQueue(const User &user) {
  const Config &config = Config::GetInstance();

  static cppkafka::Configuration cafka_config = {
    {"metadata.broker.list", config.GetQueueHost()},
    {"acks", "all"}
  };
  static cppkafka::Producer producer(cafka_config);
  static std::mutex mutex;
  static int message_key{0};

  std::lock_guard<std::mutex> lock(mutex);
  std::stringstream ss;
  Poco::JSON::Stringifier::stringify(RowToJSON(user), ss);
  std::string message = ss.str();

  //bool not_sent = true;

  cppkafka::MessageBuilder builder(config.GetQueueTopic());
  std::string mk = std::to_string(++message_key);
  // set some key
  builder.key(mk);
  // set some custom header                                       
  builder.header(
    cppkafka::MessageBuilder::HeaderType{"producer_type",
                                         "author writer"}
  ); 
  builder.payload(message);
  
  producer.produce(builder);

  // while (not_sent)
  // {
  //     try
  //     {
  //         producer.produce(builder);
  //         not_sent = false;
  //     }
  //     catch (...)
  //     {
  //     }
  // }
}

void UserConverter::FromJSON(User &user,
              const Poco::JSON::Object::Ptr &jsonObject) {
  user.set<kUserId>(
      jsonObject->getValue<uint64_t>("user_id"));
  user.set<kUserFirstName>(
      jsonObject->getValue<std::string>("first_name"));
  user.set<kUserLastName>(
      jsonObject->getValue<std::string>("last_name"));
  user.set<kUserEmail>(
      jsonObject->getValue<std::string>("email"));
  user.set<KUserLogin>(
      jsonObject->getValue<std::string>("login"));
  user.set<kUserPassword>(
      jsonObject->getValue<std::string>("password"));
}

void UserConverter::ToJSON(Poco::JSON::Object::Ptr &jsonObject,
            const User &user) {
  jsonObject->set("user_id",    user.get<kUserId>());
  jsonObject->set("first_name", user.get<kUserFirstName>());
  jsonObject->set("last_name",  user.get<kUserLastName>());
  jsonObject->set("email",      user.get<kUserEmail>());
  jsonObject->set("login",      user.get<KUserLogin>());
  jsonObject->set("password",   user.get<kUserPassword>());
}

}
