#ifndef HTTP_REQUEST_FACTORY_H_HL_MAI
#define HTTP_REQUEST_FACTORY_H_HL_MAI

#include "handlers/user_handler.h"

#include <HLMAI/contains_substr.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include <iostream>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServerRequest;

class HTTPRequestFactory: public HTTPRequestHandlerFactory
{
  public:
    HTTPRequestFactory(const std::string& format):
        format_(format)
    {}

    HTTPRequestHandler* createRequestHandler(
        const HTTPServerRequest& request) override
    {
      const std::string& uri = request.getURI();
      std::cout << "request: " << uri << std::endl;
      if (ContainsSubstr(uri, "/user") ||
          ContainsSubstr(uri, "/search") ||
          ContainsSubstr(uri, "/auth"))  {
        return new UserHandler(format_);
      }
      
      return 0;
    }
  private:
    std::string format_;
};

#endif
