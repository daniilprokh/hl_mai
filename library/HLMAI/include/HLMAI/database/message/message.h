#ifndef MESSAGE_H_HL_MAI
#define MESSAGE_H_HL_MAI

#include <Poco/Tuple.h>

#include <string>

namespace database {

using Message = Poco::Tuple<uint64_t,
                            uint64_t,
                            uint64_t,
                            std::string,
                            std::string>;

}

#endif
