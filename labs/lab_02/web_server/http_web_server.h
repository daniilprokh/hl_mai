#ifndef HTTP_WEB_SERVER_H_HL_MAI
#define HTTP_WEB_SERVER_H_HL_MAI

#include "http_request_factory.h"

#include <HLMAI/database/message/message_table.h>
#include <HLMAI/database/user/user_table.h>

#include <Poco/DateTimeFormat.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/ServerApplication.h>

using Poco::DateTimeFormat;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Util::Application;
using Poco::Util::ServerApplication;

class HTTPWebServer : public ServerApplication
{
  public:
    HTTPWebServer() : help_requested_(false)
    {}

    ~HTTPWebServer()
    {}

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
        database::UserTable::GetInstance().Create();
        ServerSocket svs(SocketAddress("0.0.0.0", 8080));
        HTTPServer srv(new HTTPRequestFactory(DateTimeFormat::SORTABLE_FORMAT),
                       svs,
                       new HTTPServerParams);
        srv.start();
        waitForTerminationRequest();
        srv.stop();
      }
      return Application::EXIT_OK;
    }
  private:
    bool help_requested_;
};

#endif