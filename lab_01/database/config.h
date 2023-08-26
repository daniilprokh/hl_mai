#pragma once

#include <string>

namespace database {

class Config {
public:
    ~Config() = default;

    static Config& GetInstance();

    const std::string& GetDatabase() const { return database_; }
    const std::string& GetHost() const { return host_; }
    const std::string& GetPort() const { return port_; }
    const std::string& GetLogin() const { return login_; }
    const std::string& GetPassword() const { return password_; }
private:
    Config();

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;

    std::string database_;
    std::string host_;
    std::string port_;
    std::string login_;
    std::string password_;
};

}
