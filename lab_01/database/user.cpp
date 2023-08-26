#include "user.h"

#include "config.h"
#include "database.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Parser.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using namespace Poco::Data::MySQL;
using Poco::Data::RecordSet;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Dynamic::Var;
using Poco::JSON::Parser;

namespace database {

User User::FromJSON(const std::string& str) {
    User user;
    Parser parser;
    Var result = parser.parse(str);
    Object::Ptr object = result.extract<Object::Ptr>();

    user.SetId(object->getValue<uint64_t>("id"));
    user.SetFirstName(object->getValue<std::string>("first_name"));
    user.SetLastName(object->getValue<std::string>("last_name"));
    user.SetEmail(object->getValue<std::string>("email"));
    user.SetLogin(object->getValue<std::string>("login"));
    user.SetPassword(object->getValue<std::string>("password"));

    return user;
}

Object::Ptr User::ToJSON() const {
    Object::Ptr root = new Object();

    root->set("id", id_);
    root->set("first_name", first_name_);
    root->set("last_name", last_name_);
    root->set("email", email_);
    root->set("login", login_);
    root->set("password", password_);

    return root;
}

void User::Init() {
    try
    {
        Database& database = Database::GetInstance();
        Session session = database.CreateSession();
        Statement create_stmt(session);
        create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                    << "`first_name` VARCHAR(256) NOT NULL,"
                    << "`last_name` VARCHAR(256) NOT NULL,"
                    << "`login` VARCHAR(256) NOT NULL,"
                    << "`password` VARCHAR(256) NOT NULL,"
                    << "`email` VARCHAR(256) NULL,"
                    << "PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));",
            now;
    }
    catch (ConnectionException &e)
    {
        std::cout << "connection: " << e.what() << std::endl;
        throw;
    }
    catch (StatementException &e)
    {

        std::cout << "statement: " << e.what() << std::endl;
        throw;
    }
}

std::vector<User> User::ReadAll() {
    try
    {
        Database& database = Database::GetInstance();
        Session session = database.CreateSession();
        Statement select(session);
        std::vector<User> result;
        User user;
        select << "SELECT "
                  "id,"
                  "first_name,"
                  "last_name,"
                  "email,"
                  "login,"
                  "password FROM User",
            into(user.id_),
            into(user.first_name_),
            into(user.last_name_),
            into(user.email_),
            into(user.login_),
            into(user.password_),
            range(0, 1); //  iterate over result set one row at a time

        while (!select.done())
        {
            if (select.execute())
                result.push_back(user);
        }
        return result;
    }
    catch (ConnectionException &e)
    {
        std::cout << "connection: " << e.what() << std::endl;
        throw;
    }
    catch (StatementException &e)
    {

        std::cout << "statement: " << e.what() << std::endl;
        throw;
    }
}

std::optional<User> User::ReadById(uint64_t id) {
    try
    {
        Database& database = Database::GetInstance();
        Session session = database.CreateSession();
        Statement select(session);
        User user;
        select << "SELECT "
                  "user_id,"
                  "first_name,"
                  "last_name,"
                  "email,"
                  "login,"
                  "password FROM users where id=?",
            into(user.id_),
            into(user.first_name_),
            into(user.last_name_),
            into(user.email_),
            into(user.login_),
            into(user.password_),
            use(id),
            range(0, 1); //  iterate over result set one row at a time

        select.execute();
        RecordSet rs(select);
        return rs.moveFirst() ? std::optional<User>(user)
                              : std::optional<User>();
    }
    catch (ConnectionException &e)
    {
        std::cout << "connection: " << e.what() << std::endl;
        throw;
    }
    catch (StatementException &e)
    {
        std::cout << "statement: " << e.what() << std::endl;
        throw;
    }
}

std::optional<uint64_t> User::Authorize(std::string login,
                                        std::string password)
{
    try
    {
        Database& database =Database::GetInstance();
        Session session = database.CreateSession();
        Statement select(session);
        long id;
        select << "SELECT "
                  "user_id FROM users where login=? and password=?",
            into(id),
            use(login),
            use(password),
            range(0, 1); //  iterate over result set one row at a time

        select.execute();
        RecordSet rs(select);
        return rs.moveFirst() ? std::optional<uint64_t>(id)
                              : std::optional<uint64_t>();
    }
    catch (ConnectionException &e)
    {
        std::cout << "connection: " << e.what() << std::endl;
        throw;
    }
    catch (StatementException &e)
    {

        std::cout << "statement: " << e.what() << std::endl;
        throw;
    }
}

std::vector<User> User::Search(std::string firstName,
                               std::string lastName) 
{
    try
    {
        Database& database = Database::GetInstance();
        Session session = database.CreateSession();
        Statement select(session);
        std::vector<User> result;
        User user;
        firstName += '%';
        lastName += '%';
        select << "SELECT "
                  "user_id,"
                  "first_name,"
                  "last_name,"
                  "email,"
                  "login,"
                  "password FROM users where first_name LIKE ? and last_name LIKE ?",
            into(user.id_),
            into(user.first_name_),
            into(user.last_name_),
            into(user.email_),
            into(user.login_),
            into(user.password_),
            use(firstName),
            use(lastName),
            range(0, 1); //  iterate over result set one row at a time

        while (!select.done())
        {
            if (select.execute())
                result.push_back(user);
        }
        return result;
    }
    catch (ConnectionException &e)
    {
        std::cout << "connection: " << e.what() << std::endl;
        throw;
    }
    catch (StatementException &e)
    {

        std::cout << "statement: " << e.what() << std::endl;
        throw;
    }
}
void User::SaveToMySQL() {
    try
    {
        Database& database = Database::GetInstance();
        Session session = database.CreateSession();
        Statement insert(session);

        insert << "INSERT INTO users "
                  "(first_name,last_name,email,login,password) "
                  "VALUES(?, ?, ?, ?, ?)",
            use(first_name_),
            use(last_name_),
            use(email_),
            use(login_),
            use(password_);

        insert.execute();

        Statement select(session);
        select << "SELECT LAST_INSERT_ID()",
            into(id_),
            range(0, 1); //  iterate over result set one row at a time

        if (!select.done())
        {
            select.execute();
        }
        std::cout << "inserted: " << id_ << std::endl;
    }
    catch (ConnectionException &e)
    {
        std::cout << "connection: " << e.what() << std::endl;
        throw;
    }
    catch (StatementException &e)
    {

        std::cout << "statement: " << e.what() << std::endl;
        throw;
    }
}

}
