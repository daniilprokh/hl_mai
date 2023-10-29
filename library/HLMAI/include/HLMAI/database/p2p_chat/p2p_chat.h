#ifndef P2P_CHAT_H_HL_MAI
#define P2P_CHAT_H_HL_MAI

#include <Poco/Tuple.h>

#include <string>

namespace database {

using P2pChat = Poco::Tuple<uint64_t,
                            uint64_t,
                            uint64_t,
                            std::string>;

}

#endif
