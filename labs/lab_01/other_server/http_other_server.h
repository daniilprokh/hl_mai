#pragma once

#include "http_other_request_factory.h"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/DateTimeFormat.h>

using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::ServerSocket;
using Poco::Util::ServerApplication;
using Poco::DateTimeFormat;

class HTTPOtherWebServer : public ServerApplication
{
public:
    HTTPOtherWebServer() : help_requested_(false)
    {
    }

    ~HTTPOtherWebServer()
    {
    }

protected:
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    int main([[maybe_unused]] const std::vector<std::string> &args)
    {
        if (!help_requested_)
        {
            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8081));
            HTTPServer srv(new HTTPOtherRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool help_requested_;
};
