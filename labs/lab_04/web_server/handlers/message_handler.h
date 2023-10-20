#ifndef MESSAGE_HANDLER_H_HL_MAI
#define MESSAGE_HANDLER_H_HL_MAI

#include <HLMAI/database/message/message_converter.h>
#include <HLMAI/database/message/message_table.h>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Timestamp.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/Exception.h>
#include <Poco/ThreadPool.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

class MessageHandler : public HTTPRequestHandler
{
  public:
    MessageHandler(const std::string &format) : _format(format)
    {}

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
      HTMLForm form(request, request.stream());
      try {
        const std::string &method = request.getMethod();
        if (method == Poco::Net::HTTPRequest::HTTP_GET) {
          if (form.has("chat_id")) {
            HandleChatRequest(form, response);
            return;
          }
        } else if (method == Poco::Net::HTTPRequest::HTTP_POST) {
          HandleSaveRequest(form, response);
          return;
        }
      
      }
      catch(...) {} 

      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
      std::ostream &ostr = response.send();
      ostr << "{ \"result\": false , \"reason\": \"exception\" }";
      response.send();
    }

  private:
    void HandleChatRequest(const HTMLForm &form, 
                           HTTPServerResponse &response) {
      uint64_t chat_id = stoull(form.get("chat_id"));

      Poco::JSON::Array arr;
      auto results = 
        database::MessageTable::GetInstance().GetChatMessages(chat_id);
      for (const auto &result : results) {
        arr.add(database::MessageConverter::GetInstance().RowToJSON(result));
      }
        
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(arr, ostr);
    }

    void HandleSaveRequest(const HTMLForm &form,
                           HTTPServerResponse &response) {
      const std::vector<std::string> properties {
          "chat_id",
          "user_id",
          "text",
          "date_time",
      };

      bool contains = true;
      for (const std::string &property : properties) {
          contains &= form.has(property);
      }
      if (contains)
      {
        database::Message message;
        message.set<database::kMessageChatId>(
            stoull(form.get(properties[0])));
        message.set<database::kMessageUserId>(
            stoull(form.get(properties[1])));
        message.set<database::kMessageText>(
            form.get(properties[2]));
        message.set<database::kMessageDateTime>(
            form.get(properties[3]));

        database::MessageTable::GetInstance().SaveToMySQL(message);

        response.setStatus(HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");

        std::ostream &ostr = response.send();
        ostr << message.get<database::kMessageId>();         
      }
    }

    std::string _format;
};

#endif