#ifndef CONFIG_H_HL_MAI
#define CONFIG_H_HL_MAI

#include "singleton.h"

#include <string>

class Config : public Singleton<Config> {
 public:
  const std::string & GetDatabaseName() const { return database_name_; }
  const std::string & GetDatabaseHost() const { return database_host_; }
  const std::string & GetDatabasePort() const { return database_port_; }
  const std::string & GetDatabaseLogin() const { return database_login_; }
  const std::string & GetDatabasePassword() const {
     return database_password_;
  }
  const std::string & GetCacheServer() const { return cache_server_; }
  const std::string & GetQueueHost() const { return queue_host_; }
  const std::string & GetQueueTopic() const { return queue_topic_; }
  const std::string & GetQueueGroupId() const { return queue_group_id_; }
  const std::string & GetSevicePort() const { return service_port_; }
 private:
  friend Singleton<Config>;

  Config()
  : Singleton<Config>(),
    database_name_(GetEnvironment("DB_DATABASE")),
    database_host_(GetEnvironment("DB_HOST")),
    database_port_(GetEnvironment("DB_PORT")),
    database_login_(GetEnvironment("DB_LOGIN")),
    database_password_(GetEnvironment("DB_PASSWORD")),
    cache_server_(GetEnvironment("CACHE")),
    queue_host_(GetEnvironment("QUEUE_HOST")),
    queue_topic_(GetEnvironment("QUEUE_TOPIC")),
    queue_group_id_(GetEnvironment("QUEUE_GROUP_ID")),
    service_port_(GetEnvironment("SERVICE_PORT"))
  {}
  ~Config() = default;

  static std::string GetEnvironment(const char *name) {
    char *environment = std::getenv(name);
    return environment ? std::string(environment)
                        : std::string();
  }

  std::string database_name_;
  std::string database_host_;
  std::string database_port_;
  std::string database_login_;
  std::string database_password_;
  std::string cache_server_;
  std::string queue_host_;
  std::string queue_topic_;
  std::string queue_group_id_;
  std::string service_port_;
};

#endif
