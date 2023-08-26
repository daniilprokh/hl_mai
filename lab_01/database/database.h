#pragma once

#include <Poco/Data/SessionPool.h>

#include <string>
#include <memory>

namespace database {

class Database {
public:
    ~Database() = default;

    static Database& GetInstance();

    Poco::Data::Session CreateSession();
private:
    Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    std::string connection_str_;
    std::unique_ptr<Poco::Data::SessionPool> pool_;
};

}
