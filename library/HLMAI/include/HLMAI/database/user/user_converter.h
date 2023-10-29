#ifndef USER_CONVERTER_H_HL_MAI
#define USER_CONVERTER_H_HL_MAI

#include "user.h"
#include "user_feature.h"

#include <HLMAI/database/row_converter.h>
#include <HLMAI/singleton.h>

#include <optional>

//TODO: cpp converters -> cache
namespace database {

class UserConverter : public RowConverter<UserConverter, User>,
                      public Singleton<UserConverter> 
{
 public:
  std::optional<User> UserFromCache(uint64_t userId);
  void UserToCache(const User &user);

  void SendToQueue(const User &user);
 private:
  friend RowConverter<UserConverter, User>;
  friend Singleton<UserConverter>;

  UserConverter() = default;
  ~UserConverter() = default;

  void FromJSON(User &user,
                const Poco::JSON::Object::Ptr &jsonObject);
  void ToJSON(Poco::JSON::Object::Ptr &jsonObject,
              const User &user);
};

}

#endif