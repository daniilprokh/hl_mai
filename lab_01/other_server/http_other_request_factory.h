#pragma once

#include "handlers/other_handler.h"

#include <Poco/Net/HTTPRequestHandlerFactory.h>

using Poco::Net::HTTPRequestHandlerFactory;

class HTTPOtherRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPOtherRequestFactory(const std::string &format) : format_(format)
    {
    }

    HTTPRequestHandler *createRequestHandler(
        [[maybe_unused]] const HTTPServerRequest &request) override
    {

        return new OtherHandler(format_);
    }

private:
    std::string format_;
};
