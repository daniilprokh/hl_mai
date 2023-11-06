#ifndef GROUP_CHAT_HANDLER_H_HL_MAI
#define GROUP_CHAT_HANDLER_H_HL_MAI

#include <HLMAI/database/chat_user/chat_user_feature.h>
#include <HLMAI/database/chat_user/chat_user_table.h>
#include <HLMAI/database/group_chat/group_chat_feature.h>
#include <HLMAI/database/group_chat/group_chat_message_table.h>
#include <HLMAI/database/group_chat/group_chat_table.h>
#include <HLMAI/database/message/message_converter.h>
#include <HLMAI/database/message/message_feature.h>
#include <HLMAI/check_html_form.h>
#include <HLMAI/contains_substr.h>

#include <Poco/JSON/Object.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class GroupChatHandler : public Poco::Net::HTTPRequestHandler {
 public:
  GroupChatHandler(const std::string &format) : format_(format)
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
        } else if (ContainsSubstr(uri, "/add/user")) {
          HandleAdditionUserRequest(form, response);
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
    response.setContentType("application/json");

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("detail", "request not found");
    root->set("instance", "/group_chat");

    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
  }
 private:
  void HandleAdditionMessageRequest(const Poco::Net::HTMLForm &form,
                                    Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
      "chat_id",
      "user_id",
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

    auto &group_chat_message_table =
        database::GroupChatMessageTable::GetInstance();
    std::string text = form.get(properties[2]);
    auto chat_id = group_chat_message_table.AddChatMessage(
        stoull(form.get(properties[0])),
        stoull(form.get(properties[1])),
        text
    );

    if (chat_id.has_value()) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("application/json");

      std::ostream &ostr = response.send();
      ostr << chat_id.value();
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

  void HandleAdditionUserRequest(const Poco::Net::HTMLForm &form,
                                 Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
      "user_id",
      "chat_id"
    };
    std::string bad_request_message;
    if (CheckHtmlForm(form, properties, bad_request_message)) {
      database::ChatUser chat_user;
      chat_user.set<database::kChatUserId>(
          std::stoull(form.get(properties[0])));
      chat_user.set<database::kChatUserChatId>(
          std::stoull(form.get(properties[1])));

      database::ChatUserTable::GetInstance().SaveToMySQL(chat_user);
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("application/json");
      response.send();
    } else {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/add/user");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    }
  }

  void HandleCreationRequest(const Poco::Net::HTMLForm &form,
                             Poco::Net::HTTPServerResponse &response) {
    const std::string property = "title";
    std::string bad_request_message;
    if (CheckHtmlForm(form, property, bad_request_message)) {
      database::GroupChat group_chat;
      group_chat.set<database::kGroupChatTitle>(form.get(property));
      database::GroupChatTable::GetInstance().SaveToMySQL(group_chat);

      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("application/json");

      std::ostream &ostr = response.send();
      ostr << group_chat.get<database::kGroupChatId>(); 
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

  void HandleLoadRequest(const Poco::Net::HTMLForm &form,
                         Poco::Net::HTTPServerResponse &response) {
    const std::string property = "chat_id";
    std::string bad_request_message;
    if (!CheckHtmlForm(form, property, bad_request_message)) {
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

    auto &group_chat_message_table = 
        database::GroupChatMessageTable::GetInstance();
    auto results = group_chat_message_table.GetChatMessages(
        stoull(form.get(property)));
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
      for (auto &result : results) {
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