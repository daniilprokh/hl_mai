#ifndef HTTP_OTHER_REQUEST_FACTORY_H_HL_MAI
#define HTTP_OTHER_REQUEST_FACTORY_H_HL_MAI

#include "other_handler.h"

#include <Poco/Net/HTTPRequestHandlerFactory.h>

class HTTPOtherRequestFactory : public Poco::Net::HTTPRequestHandlerFactory
{
 public:
  HTTPOtherRequestFactory(const std::string &format) : format_(format)
  {}

  Poco::Net::HTTPRequestHandler *createRequestHandler(
      [[maybe_unused]] const Poco::Net::HTTPServerRequest &request) override
  {
    return new OtherHandler(format_);
  }

 private:
  std::string format_;
};

#endif
