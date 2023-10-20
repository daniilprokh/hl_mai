#ifndef CONTAINS_SUBSTR_H_HL_MAI
#define CONTAINS_SUBSTR_H_HL_MAI

#include <string_view>

inline bool ContainsSubstr(std::string_view str, std::string_view substr) {
    if (str.size() < substr.size()) {
        return false;
    }
        
    for (size_t idx = 0, diff = str.size() - substr.size();
        idx <= diff;
        idx += 1)
    {
        bool result = true;
        for (size_t jdx = 0; result && (jdx < substr.size()); jdx += 1) {
            result = (str[idx + jdx] == substr[jdx]);
        }
        if (result) {
            return true;
        }   
    }

    return false;
}

#endif