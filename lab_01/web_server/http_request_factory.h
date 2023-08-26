#pragma once

#include "handlers/user_handler.h"

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
        if (utility::ContainsSubstr(uri,"/user") ||
            utility::ContainsSubstr(uri,"/search") ||
            utility::ContainsSubstr(uri,"/auth"))  {
            return new UserHandler(format_);
        }
           
        return 0;
    }
private:
    std::string format_;
};
