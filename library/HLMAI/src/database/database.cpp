#include <HLMAI/database/database.h>
#include <HLMAI/config.h>

#include <Poco/Data/MySQL/Connector.h>

using Poco::Data::MySQL::Connector;

namespace database {

Database::Database() {
  Config& config = Config::GetInstance();

  connection_str_ += "host=";
  connection_str_ += config.GetHost();
  connection_str_ += ";user=";
  connection_str_ += config.GetLogin();
  connection_str_ += ";db=";
  connection_str_ += config.GetDatabase();
  connection_str_ += ";port=";
  connection_str_ += config.GetPort();
  connection_str_ += ";password=";
  connection_str_ += config.GetPassword();

  std::cout << connection_str_ << std::endl; 

  Connector::registerConnector();
  pool_ = std::make_unique<Poco::Data::SessionPool>(Connector::KEY,
                                                    connection_str_);
}

std::string Database::FormShardingName(uint8_t nodeIdx) {
  return "-- sharding:" + std::to_string(nodeIdx);
}

std::vector<std::string> Database::GetShardingHints() {
  std::vector<std::string> sharding_hints;
  for (auto node_index = 0; node_index < kNodeCount; node_index += 1) {
    std::string sharding_hint = FormShardingName(node_index);
    sharding_hints.push_back(std::move(sharding_hint));
  }
  return sharding_hints;
}

std::string Database::GetUserShardingHint(uint64_t userId) {
  std::string user_key = std::to_string(userId) + ';';
  uint8_t node_index = std::hash<std::string>{}(user_key) % kNodeCount;
  return FormShardingName(node_index);
}

}
