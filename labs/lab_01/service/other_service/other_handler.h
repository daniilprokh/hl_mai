#ifndef OTHER_HANDLER_H_HL_MAI
#define OTHER_HANDLER_H_HL_MAI

#include <HLMAI/identity.h>

#include <Poco/JSON/Object.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Base64Encoder.h>
#include <Poco/URI.h>

#include <optional>

class OtherHandler : public Poco::Net::HTTPRequestHandler
{
 public:
  OtherHandler(const std::string &format) : format_(format)
  {}

  std::optional<std::string> PerformAuthentication(
      const std::string &url,
      const std::string &login,
      const std::string &password)
  {
    try
    {
      std::string token = login + ":" + password;
      std::ostringstream os;
      Poco::Base64Encoder b64in(os);
      b64in << token;
      b64in.close();
      std::string identity = "Basic " + os.str();

      Poco::URI uri(url);
      Poco::Net::HTTPClientSession s(uri.getHost(), uri.getPort());

      Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET,
                                     uri.toString());
      request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
      request.setContentType("application/json");
      request.set("Authorization", identity);
      request.set("Accept", "application/json");
      request.setKeepAlive(true);

      s.sendRequest(request);

      Poco::Net::HTTPResponse response;
      std::istream &rs = s.receiveResponse(response);

      std::string result(std::istreambuf_iterator<char>(rs), {});

      return response.getStatus() == 200 
          ? std::optional<std::string>(result)
          : std::optional<std::string>();
    }
    catch (Poco::Exception &ex)
    {
        std::cout << "exception: " << ex.what() << std::endl;
        return std::optional<std::string>();
    }
  }

  void handleRequest(Poco::Net::HTTPServerRequest &request,
                     Poco::Net::HTTPServerResponse &response) override
  {
    Poco::Net::HTMLForm form(request, request.stream());
    std::string scheme;
    std::string info;
    std::string login, password;

    request.getCredentials(scheme, info);
    if (scheme == "Basic")
    {
      GetIdentity(info, login, password);
      std::cout << "login: " << login << std::endl;
      std::cout << "password: "  << password << std::endl;
      std::string host = std::getenv("SERVICE_HOST");
      if (host.empty()) {
          host = "localhost";
      }
      std::string url = "http://" + host + ":8080/auth";

      std::optional<std::string> result = PerformAuthentication(url, login, password);
      if (result.has_value()) // do authentificate
      {
          response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
          response.setContentType("application/json");

          Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
          root->set("result", result.value());

          std::ostream &ostr = response.send();
          Poco::JSON::Stringifier::stringify(root, ostr);
          return;
      }
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/unauthorized");
    root->set("title", "Unauthorized");
    root->set("status", "401");
    root->set("detail", "invalid login or password");
    root->set("instance", "/user");

    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
  }
private:
  std::string format_;
};

#endif
