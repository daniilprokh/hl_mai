#include "config.h"

namespace database {

Config::Config()
:   database_(std::getenv("DB_DATABASE")),
    host_(std::getenv("DB_HOST")),
    port_(std::getenv("DB_PORT")),
    login_(std::getenv("DB_LOGIN")),
    password_(std::getenv("DB_PASSWORD"))
{}

Config& Config::GetInstance() {
    static Config instance;
    return instance; 
}

}
