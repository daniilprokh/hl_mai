#ifndef WEB_PAGE_HANDLER_H_HL_MAI
#define WEB_PAGE_HANDLER_H_HL_MAI

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <fstream>
#include <ostream>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

class WebPageHandler: public HTTPRequestHandler
{
public:
    WebPageHandler(const std::string& format): _format(format)
    {}

    void handleRequest(HTTPServerRequest& request,
                       HTTPServerResponse& response)
    {
      response.setChunkedTransferEncoding(true);
      response.setContentType("text/html");

      std::ostream& ostr = response.send();

      const std::string &uri = request.getURI();
      size_t pos = uri.find('?');
      
      std::string name = "content" + 
          (pos == std::string::npos ? uri : uri.substr(0, pos));
      std::ifstream file(name, std::ifstream::binary);
      if (!file.is_open()) {
        return;
      }

      while (file.good()) {
        int sign = file.get();
        if (sign > 0) {
          ostr << static_cast<char>(sign);
        }
      }
      
      file.close();
    }

private:
    std::string _format;
};

#endif
