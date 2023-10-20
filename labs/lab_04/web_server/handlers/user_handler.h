#ifndef USER_HANDLER_H_HL_MAI
#define USER_HANDLER_H_HL_MAI

#include <HLMAI/database/user/user_converter.h>
#include <HLMAI/database/user/user_table.h>
#include <HLMAI/identity.h>
#include <HLMAI/contains_substr.h>

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

#include <iostream>
#include <sstream>

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

class UserHandler : public HTTPRequestHandler
{
  public:
    UserHandler(const std::string &format) : format_(format)
    {}

    bool RemovePassword(Poco::JSON::Object::Ptr src)
    {
      if (src->has("password")) {
        src->set("password", "*******");
        return true;
      } else {
        return false;
      }
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response) override
    {
      HTMLForm form(request, request.stream());
      try
      {
        const std::string &method = request.getMethod();
        const std::string &uri = request.getURI();
        if (method == HTTPRequest::HTTP_GET) {
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
        } else if (method == HTTPRequest::HTTP_POST) {
          HandleSaveRequest(form, response);
          return;
        }
      }
      catch (...) {}

      response.setStatus(HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/not_found");
      root->set("title", "Internal exception");
      root->set("detail", "request ot found");
      root->set("instance", "/user");

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

    void HandleUserRequest(const HTMLForm &form,
                           HTTPServerResponse &response) {
      uint64_t id = stoull(form.get("user_id"));

      bool cached = form.has("cach");

      if (cached) {
        auto user = database::UserConverter::GetInstance().UserFromCache(id);
        if (user.has_value()) {
          response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
          response.setChunkedTransferEncoding(true);
          response.setContentType("application/json");
          std::ostream &ostr = response.send();
          
          auto result_json = 
              database::UserConverter::GetInstance().RowToJSON(user.value());
          RemovePassword(result_json);

          Poco::JSON::Stringifier::stringify(result_json, ostr);
          return;
        }
      }

      auto result = database::UserTable::GetInstance().ReadById(id);
      if (result.has_value())
      {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        
        auto result_json = 
            database::UserConverter::GetInstance().RowToJSON(result.value());
        RemovePassword(result_json);

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
        root->set("detail", "user ot found");
        root->set("instance", "/user");
        
        Poco::JSON::Stringifier::stringify(root, ostr);
      }
    }

    void HandleAuthorizationRequest(const HTTPServerRequest &request,
                                    HTTPServerResponse &response) {
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
          response.setStatus(HTTPResponse::HTTP_OK);
          response.setChunkedTransferEncoding(true);
          response.setContentType("application/json");

          std::ostream &ostr = response.send();
          ostr << "{ \"user_id\" : \"" << id.value() << "\"}" << std::endl;
          return;
        }
      }

      response.setStatus(HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");

      Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
      root->set("type", "/errors/unauthorized");
      root->set("title", "Internal exception");
      root->set("detail", "not authorized");
      root->set("instance", "/auth");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(root, ostr);
    }

    void HandleSearchRequest(const HTMLForm &form,
                             HTTPServerResponse &response) {
      std::string fn = form.get("first_name");
      std::string ln = form.get("last_name");
      auto results = 
          database::UserTable::GetInstance().Search(fn, ln);
      Poco::JSON::Array arr;
      for (const auto &result : results) {
        auto result_json = 
            database::UserConverter::GetInstance().RowToJSON(result);
        RemovePassword(result_json);
        arr.add(result_json);
      }
          
      response.setStatus(HTTPResponse::HTTP_OK);
      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");

      std::ostream &ostr = response.send();
      Poco::JSON::Stringifier::stringify(arr, ostr);
    }

    void HandleSaveRequest(const HTMLForm &form,
                           HTTPServerResponse &response) {
      const std::vector<std::string> properties {
          "first_name",
          "last_name",
          "email",
          "login",
          "password"
      };

      bool contains = true;
      for (const std::string &property : properties) {
          contains &= form.has(property);
      }
      if (contains)
      {
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
          //database::UserTable::GetInstance().SaveToMySQL(user);

          database::UserConverter::GetInstance().UserToCache(user);
          database::UserConverter::GetInstance().SendToQueue(user);

          response.setStatus(HTTPResponse::HTTP_OK);
          response.setChunkedTransferEncoding(true);
          response.setContentType("application/json");

          std::ostream &ostr = response.send();
          ostr << user.get<database::kUserId>();         
        }
        else
        {
          response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
          std::ostream &ostr = response.send();
          ostr << message;
        }  
      }
    }

    std::string format_;
};

#endif
