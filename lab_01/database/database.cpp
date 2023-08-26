#include "database.h"

#include "config.h"

#include <Poco/Data/MySQL/Connector.h>

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

    Poco::Data::MySQL::Connector::registerConnector();
    pool_ = std::make_unique<Poco::Data::SessionPool>(
        Poco::Data::MySQL::Connector::KEY, connection_str_
    );
}

Database& Database::GetInstance() {
    static Database instance;
    return instance;
}

Poco::Data::Session Database::CreateSession() {
    return Poco::Data::Session(pool_->get());
}

}
