#ifndef GROUP_CHAT_H_HL_MAI
#define GROUP_CHAT_H_HL_MAI

#include <Poco/Tuple.h>

#include <string>

namespace database {

using GroupChat = Poco::Tuple<uint64_t, std::string>;

}

#endif