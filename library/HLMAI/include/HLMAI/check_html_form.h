#ifndef CHECK_HTML_FORM_H_HL_MAI
#define CHECK_HTML_FORM_H_HL_MAI

#include <Poco/Net/HTMLForm.h>

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

inline bool CheckHtmlForm(const Poco::Net::HTMLForm &form,
                          const std::string &property,
                          std::string &message) {
  if (form.has(property)) {
    return true;
  } else {
    std::ostringstream oss;
    oss << "parameters " << std::quoted(property, '\'') << " not found";
    message = oss.str();
    return false;
  }
}

inline bool CheckHtmlForm(const Poco::Net::HTMLForm &form,
                          const std::vector<std::string> &properties,
                          std::string &message) {
  std::vector<std::string> bad_properties;
  for (const std::string &property : properties) {
    if (!form.has(property)) {
      bad_properties.push_back(property);
    }
  }
  if (bad_properties.empty()) {
    return true;
  }

  std::ostringstream oss;
  oss << "parameter";
  if (bad_properties.size() == 1) {
    oss << ' ';
  } else {
    oss << "s ";
  }
  oss << std::quoted(bad_properties.front(), '\'');
  for (size_t idx = 1; idx < bad_properties.size(); idx += 1) {
    oss << ' ' << std::quoted(bad_properties[idx], '\'');
  }
  message = oss.str();

  return false;
}

#endif
