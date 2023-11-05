#ifndef HTTP_WEB_SERVER_H_HL_MAI
#define HTTP_WEB_SERVER_H_HL_MAI

#include <HLMAI/config.h>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/DateTimeFormat.h>

template <class RequestFactory>
class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
  HTTPWebServer() = default;
  ~HTTPWebServer() = default;

protected:
  void initialize(Poco::Util::Application &self)
  {
    loadConfiguration();
    Poco::Util::ServerApplication::initialize(self);
  }

  void uninitialize()
  {
    Poco::Util::ServerApplication::uninitialize();
  }

  int main([[maybe_unused]] const std::vector<std::string> &args)
  {
    Config &config = Config::GetInstance();

    Poco::Net::ServerSocket svs(
        Poco::Net::SocketAddress("0.0.0.0",
                                 std::stoi(config.GetSevicePort()))
    );
    Poco::Net::HTTPServer srv(
        new RequestFactory(Poco::DateTimeFormat::SORTABLE_FORMAT),
        svs,
        new Poco::Net::HTTPServerParams
    );
    srv.start();
    waitForTerminationRequest();
    srv.stop();

    return Poco::Util::Application::EXIT_OK;
  }
};

#endif