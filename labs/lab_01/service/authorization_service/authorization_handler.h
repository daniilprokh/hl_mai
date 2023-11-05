#ifndef AUTHORIZATION_HANDLER_H_HL_MAI
#define AUTHORIZATION_HANDLER_H_HL_MAI

#include <HLMAI/database/user/user_converter.h>
#include <HLMAI/database/user/user_table.h>
#include <HLMAI/identity.h>
#include <HLMAI/check_html_form.h>
#include <HLMAI/contains_substr.h>

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <iostream>
#include <sstream>

class AutorizationHandler : public Poco::Net::HTTPRequestHandler {
 public:
  AutorizationHandler(const std::string &format) : format_(format)
  {}

  void handleRequest(Poco::Net::HTTPServerRequest &request,
                     Poco::Net::HTTPServerResponse &response) override
  {
    Poco::Net::HTMLForm form(request, request.stream());
    try
    {
      const std::string &method = request.getMethod();
      const std::string &uri = request.getURI();
      if (method == Poco::Net::HTTPRequest::HTTP_GET) {
        if (ContainsSubstr(uri, "/user")) {
          HandleUserRequest(form, response);
          return;
        }
        else if (ContainsSubstr(uri, "/auth"))
        {
          HandleAuthorizationRequest(request, response);
          return;
        }
        else if (ContainsSubstr(uri, "/search"))
        {
          HandleSearchRequest(form, response);
          return;
        }
      } else if (method == Poco::Net::HTTPRequest::HTTP_POST) {
        if (ContainsSubstr(uri, "/user")) {
          HandleSaveRequest(form, response);
          return;
        }
        
      }
    }
    catch (...)
    {}

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setContentType("application/json");

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/http_not_found");
    root->set("title", "Internal exception");
    root->set("detail", "request not found");
    //root->set("instance", "/user");

    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
  }
 private:
  class Checker {
   public:
    void Check(const database::User &user, std::string &message) {
      CheckName(user.get<database::kUserFirstName>());
      CheckName(user.get<database::kUserLastName>());
      CheckEmail(user.get<database::kUserEmail>());

      message = std::move(error_message_);
    }
   private:
    void AddError(const char *error) {
      error_message_ += error;
      error_message_ += "<br>";
    }

    void CheckName(const std::string &name) {
      if (name.length() < 3)
      {
        AddError("Name must be at least 3 signs");
        return;
      }
      
      if (name.find(' ') != std::string::npos ||
          name.find('\t') != std::string::npos)
      {
        AddError("Name can't contain spaces");
      }
    }

    void CheckEmail(const std::string &email) {
      if (email.find('@') == std::string::npos)
      {
        AddError("Email must contain @");
        return;
      }

      if (email.find(' ') != std::string::npos ||
          email.find('\t') != std::string::npos)
      {
        AddError("Email can't contain spaces");
        return;
      }
    }

    std::string error_message_;
  };

  void HandleUserRequest(const Poco::Net::HTMLForm &form,
                         Poco::Net::HTTPServerResponse &response) {
    const std::string property = "user_id";
    std::string bad_request_message;
    if (!CheckHtmlForm(form, property, bad_request_message)) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/user");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
      return;
    }

    uint64_t id = stoull(form.get(property));
    std::optional<database::User> result =
        database::UserTable::GetInstance().ReadById(id);
    if (result.has_value())
    {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");
      
      auto result_json = 
          database::UserConverter::GetInstance().RowToJSON(result.value());
      RemovePassword(result_json);

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(result_json, ostr);
    }
    else
    {
      response.setStatus(
          Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");
      std::ostream &ostr = response.send();

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "user not found");
      root->set("instance", "/user");
      
      Poco::JSON::Stringifier::stringify(root, ostr);
    }
  }

  void HandleAuthorizationRequest(const Poco::Net::HTTPServerRequest &request,
                                  Poco::Net::HTTPServerResponse &response) {
    std::string scheme;
    std::string info;
    request.getCredentials(scheme, info);
    std::cout << "scheme: " << scheme
              << " identity: " << info << std::endl;

    std::string login, password;
    if (scheme == "Basic")
    {
      GetIdentity(info, login, password);
      std::optional<uint64_t> id = 
          database::UserTable::GetInstance().Authorize(login, password);
      if (id.has_value())
      {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");

        std::ostream &ostr = response.send();
        ostr << id.value() << std::endl;
        return;
      }
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
    response.setContentType("application/json");

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/unauthorized");
    root->set("title", "Internal exception");
    root->set("detail", "no authorized");
    root->set("instance", "/auth");

    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
  }

  void HandleSearchRequest(const Poco::Net::HTMLForm &form,
                           Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
      "first_name",
      "last_name"
    };
    std::string bad_request_message;
    if (!CheckHtmlForm(form, properties, bad_request_message)) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/search");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
      return;
    }

    std::string fn = form.get(properties[0]);
    std::string ln = form.get(properties[1]);
    auto results = database::UserTable::GetInstance().Search(fn, ln);
    if (results.empty()) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "users not found");
      root->set("instance", "/search");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    } else {
      Poco::JSON::Array arr;
      for (const auto &result : results) {
        auto result_json = 
            database::UserConverter::GetInstance().RowToJSON(result);
        RemovePassword(result_json);
        arr.add(result_json);
      }
          
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("application/json");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(arr, ostr);
    }
  }

  void HandleSaveRequest(const Poco::Net::HTMLForm &form,
                         Poco::Net::HTTPServerResponse &response) {
    const std::vector<std::string> properties {
      "first_name",
      "last_name",
      "email",
      "login",
      "password"
    };
    std::string bad_request_message;
    if (CheckHtmlForm(form, properties, bad_request_message)) {
      database::User user;
      user.set<database::kUserFirstName>(
          form.get(properties[0]));
      user.set<database::kUserLastName>(
          form.get(properties[1]));
      user.set<database::kUserEmail>(
          form.get(properties[2]));
      user.set<database::KUserLogin>(
          form.get(properties[3]));
      user.set<database::kUserPassword>(
          form.get(properties[4]));

      std::string message;
      Checker cheker;
      cheker.Check(user, message);
      if (message.empty())
      {
        database::UserTable::GetInstance().SaveToMySQL(user);

        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setContentType("application/json");

        std::ostream &ostr = response.send();
        ostr << user.get<database::kUserId>() << std::endl;         
      }
      else
      {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);

        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("detail", message);
        root->set("instance", "/user");

        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
      }  
    } else {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/bad_request");
      root->set("title", "Internal exception");
      root->set("detail", bad_request_message);
      root->set("instance", "/user");
      
      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    }
  }

  bool RemovePassword(Poco::JSON::Object::Ptr& src)
  {
    if (src->has("password")) {
      src->set("password", "*******");
      return true;
    } else {
      return false;
    }
  }

  std::string format_;
};

#endif