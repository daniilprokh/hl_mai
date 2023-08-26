#pragma once

#include <Poco/JSON/Object.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

using Poco::JSON::Object;

namespace database {

class User {
public:
    User() = default;

    User(const User&) = default;
    User& operator=(const User&) = default;

    User(User&&) = default;
    User& operator=(User&&) = default;

    ~User() = default;

    static User FromJSON(const std::string& str);

    static void Init();

    static std::vector<User> ReadAll();
    static std::optional<User> ReadById(uint64_t id);
    static std::optional<uint64_t> Authorize(std::string login,
                                             std::string password);
    static std::vector<User> Search(std::string firstName,
                                    std::string lastName);
    void SaveToMySQL();

    Object::Ptr ToJSON() const;

    uint64_t GetId() const { return id_; }
    const std::string& GetFirstName() const { return first_name_; }
    const std::string& GetLastName() const { return last_name_; }
    const std::string& GetEmail() const { return email_; }
    const std::string& GetLogin() const { return login_; }
    const std::string& GetPassword() const { return password_; }

    void SetId(uint64_t id) { id_ = id; }
    void SetFirstName(const std::string& firstName) { first_name_ = firstName; }
    void SetLastName(const std::string& lastName) { last_name_ = lastName; }
    void SetEmail(const std::string& email) { email_ = email; }
    void SetLogin(const std::string& login) { login_ = login; }
    void SetPassword(const std::string& password) { password_ = password; }
private:
    uint64_t id_;

    std::string first_name_;
    std::string last_name_;
    std::string email_;

    std::string login_;
    std::string password_;
};

}
