#ifndef CONFIG_H_HL_MAI
#define CONFIG_H_HL_MAI

#include "singleton.h"

#include <string>

class Config : public Singleton<Config> {
  public:
    const std::string & GetDatabase() const { return database_; }
    const std::string & GetHost() const { return host_; }
    const std::string & GetPort() const { return port_; }
    const std::string & GetLogin() const { return login_; }
    const std::string & GetPassword() const { return password_; }
    const std::string & GetCacheServer() const { return cache_server_; }
    const std::string & GetQueueHost() const { return queue_host_; }
    const std::string & GetQueueTopic() const { return queue_topic_; }
    const std::string & GetQueueGroupId() const { return queue_group_id_; }

  private:
    friend Singleton<Config>;

    Config()
    : Singleton<Config>(),
      database_(GetEnvironment("DB_DATABASE")),
      host_(GetEnvironment("DB_HOST")),
      port_(GetEnvironment("DB_PORT")),
      login_(GetEnvironment("DB_LOGIN")),
      password_(GetEnvironment("DB_PASSWORD")),
      cache_server_(GetEnvironment("CACHE")),
      queue_host_(GetEnvironment("QUEUE_HOST")),
      queue_topic_(GetEnvironment("QUEUE_TOPIC")),
      queue_group_id_(GetEnvironment("QUEUE_GROUP_ID"))
    {}
    ~Config() = default;

    static std::string GetEnvironment(const char *name) {
      char *environment = std::getenv(name);
      return environment ? std::string(environment)
                         : std::string();
    }

    std::string database_;
    std::string host_;
    std::string port_;
    std::string login_;
    std::string password_;
    std::string cache_server_;
    std::string queue_host_;
    std::string queue_topic_;
    std::string queue_group_id_;
};

#endif
