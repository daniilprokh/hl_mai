#pragma once

#include <string>

namespace utility {

bool ContainsSubstr(const std::string& str, const std::string& substr) {
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

}
