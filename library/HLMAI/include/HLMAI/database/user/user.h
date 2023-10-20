#ifndef USER_H_HL_MAI
#define USER_H_HL_MAI

#include <Poco/Tuple.h>

#include <string>

namespace database {

using User = Poco::Tuple<uint64_t,
                         std::string,
                         std::string,
                         std::string,
                         std::string,
                         std::string>;

}

#endif