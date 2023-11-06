#ifndef P2P_CHAT_HANDLER_H_HL_MAI
#define P2P_CHAT_HANDLER_H_HL_MAI

#include <HLMAI/database/message/message_converter.h>
#include <HLMAI/database/message/message_feature.h>
#include <HLMAI/database/p2p_chat/p2p_chat_feature.h>
#include <HLMAI/database/p2p_chat/p2p_chat_table.h>
#include <HLMAI/database/p2p_chat/p2p_chat_message_table.h>
#include <HLMAI/check_html_form.h>
#include <HLMAI/contains_substr.h>

#include <Poco/JSON/Object.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class P2pChatHandler : public Poco::Net::HTTPRequestHandler {
 public:
  P2pChatHandler(const std::string &format) : format_(format)
  {}

  void handleRequest(Poco::Net::HTTPServerRequest &request,
                     Poco::Net::HTTPServerResponse &response) override
  {
    Poco::Net::HTMLForm form(request, request.stream());
    try {
      const std::string &method = request.getMethod();
      const std::string &uri = request.getURI();
      if (method == Poco::Net::HTTPRequest::HTTP_GET) {
        if (ContainsSubstr(uri, "/load")) {
          HandleLoadRequest(form, response);
          return;
        }
      } else if (method == Poco::Net::HTTPRequest::HTTP_POST) {
        if (ContainsSubstr(uri, "/create")) {
          HandleCreationRequest(form, response);
          return;
        } else if (ContainsSubstr(uri, "/add/message")) {
          HandleAdditionMessageRequest(form, response);
          return;
        }
      }
    }
    catch(...)
    {}

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("detail", "request not found");
    root->set("instance", "/p2p_chat");

    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
  }
 private:
  void HandleCreationRequest(const Poco::Net::HTMLForm &form,
                              Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
        "user_id1",
        "user_id2"
    };
    std::string bad_request_message;
    if (CheckHtmlForm(form, properties, bad_request_message)) {
      database::P2pChat p2p_chat;
      p2p_chat.set<database::kP2pChatUserId1>(
          stoull(form.get(properties[0])));
      p2p_chat.set<database::kP2pChatUserId2>(
          stoull(form.get(properties[1])));

      database::P2pChatTable::GetInstance().SaveToMySQL(p2p_chat);
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("application/json");

      std::ostream &ostr = response.send();
      ostr << p2p_chat.get<database::kP2pChatId>(); 
    } else {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/create");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    }
  }

  void HandleAdditionMessageRequest(const Poco::Net::HTMLForm &form,
                                    Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
        "sender_id",
        "receiver_id",
        "text"
    };
    std::string bad_request_message;
    if (!CheckHtmlForm(form, properties, bad_request_message)) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/add/message");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);

      return;
    }

    auto chat_id = database::P2pChatTable::GetInstance().GetP2pChatId(
        stoull(form.get(properties[0])),
        stoull(form.get(properties[1]))
    );

    if (!chat_id.has_value()) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "p2p chat not found");
      root->set("instance", "/add/message");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);

      return;
    }

    auto &p2p_chat_message_table = 
        database::P2pChatMessageTable::GetInstance();
    std::string text = form.get(properties[2]);
    auto message_id = p2p_chat_message_table.AddChatMessage(
        chat_id.value(),
        stoull(form.get(properties[0])),
        text
    );
    if (message_id.has_value()) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("application/json");

      std::ostream &ostr = response.send();
      ostr << message_id.value();
    } else {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "message not added");
      root->set("instance", "/add/message");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    }
  }

  void HandleLoadRequest(const Poco::Net::HTMLForm &form,
                         Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
        "user_id1",
        "user_id2"
    };
    std::string bad_request_message;
    if (!CheckHtmlForm(form, properties, bad_request_message)) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/load");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);

      return;
    }

    auto chat_id = database::P2pChatTable::GetInstance().GetP2pChatId(
        stoull(form.get(properties[0])),
        stoull(form.get(properties[1]))
    );
    if (!chat_id.has_value()) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "p2p chat not found");
      root->set("instance", "/load");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);

      return;
    }

    auto &p2p_chat_message_table = 
        database::P2pChatMessageTable::GetInstance();
    auto results = p2p_chat_message_table.GetChatMessages(chat_id.value());
    if (results.empty()) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "messages not found");
      root->set("instance", "/load");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    } else {
      Poco::JSON::Array arr;
      auto &message_converter = database::MessageConverter::GetInstance();
      for (auto& result : results) {
        arr.add(message_converter.RowToJSON(result));
      }
      
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(arr, ostr);
    }
  }

  std::string format_;
};

#endif