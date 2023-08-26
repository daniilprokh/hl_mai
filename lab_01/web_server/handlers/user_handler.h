#pragma once

#include "../../database/user.h"
#include "identity.h"
#include "substr_check.h"

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
            const std::string& method = request.getMethod();
            if (method == HTTPRequest::HTTP_GET) {
                if (form.has("user_id")) {
                    uint64_t id = stoull(form.get("user_id"));

                    std::optional<database::User> result = database::User::ReadById(id);
                    if (result.has_value())
                    {
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        
                        auto result_json = result->ToJSON();
                        RemovePassword(result_json);

                        Poco::JSON::Stringifier::stringify(result_json, ostr);
                    }
                    else
                    {
                        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();

                        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                        root->set("type", "/errors/not_found");
                        root->set("title", "Internal exception");
                        root->set("status", "404");
                        root->set("detail", "user ot found");
                        root->set("instance", "/user");
                        
                        Poco::JSON::Stringifier::stringify(root, ostr);
                    }
                    return;
                }
                const std::string& uri = request.getURI();
                if (utility::ContainsSubstr(uri, "/auth"))
                {
                    std::string scheme;
                    std::string info;
                    request.getCredentials(scheme, info);
                    std::cout << "scheme: " << scheme << " identity: " << info << std::endl;

                    std::string login, password;
                    if (scheme == "Basic")
                    {
                        utility::GetIdentity(info, login, password);
                        std::optional<uint64_t> id = database::User::Authorize(login, password);
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
                    root->set("status", "401");
                    root->set("detail", "not authorized");
                    root->set("instance", "/auth");

                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
                else if (utility::ContainsSubstr(uri, "/search"))
                {
                    std::string fn = form.get("first_name");
                    std::string ln = form.get("last_name");
                    auto results = database::User::Search(fn, ln);
                    Poco::JSON::Array arr;
                    for (const auto& result : results) {
                        auto result_json = result.ToJSON();
                        RemovePassword(result_json);
                        arr.add(result_json);
                    }
                        
                    response.setStatus(HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");

                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(arr, ostr);
                    return;
                }
            } else if (method == HTTPRequest::HTTP_POST) {
                const std::vector<std::string> properties {
                    "first_name",
                    "last_name",
                    "email",
                    "login",
                    "password"
                };

                bool contains = true;
                for (const auto& property : properties) {
                    contains &= form.has(property);
                }
                if (contains)
                {
                    database::User user;
                    user.SetFirstName(form.get(properties[0]));
                    user.SetLastName(form.get(properties[1]));
                    user.SetEmail(form.get(properties[2]));
                    user.SetLogin(form.get(properties[3]));
                    user.SetPassword(form.get(properties[4]));

                    std::string message;
                    Checker cheker;
                    cheker.Check(user, message);
                    if (message.empty())
                    {
                        user.SaveToMySQL();

                        response.setStatus(HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");

                        std::ostream &ostr = response.send();
                        ostr << user.GetId();         
                    }
                    else
                    {
                        response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
                        std::ostream &ostr = response.send();
                        ostr << message;
                        response.send();
                    }
                    return;
                }
            }

            // if (form.has("id") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
            // {
            //     long id = atol(form.get("id").c_str());

            //     std::optional<database::User> result = database::User::ReadById(id);
            //     if (result.has_value())
            //     {
            //         response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            //         response.setChunkedTransferEncoding(true);
            //         response.setContentType("application/json");
            //         std::ostream &ostr = response.send();
                    
            //         auto result_json = result->ToJSON();
            //         RemovePassword(result_json);

            //         Poco::JSON::Stringifier::stringify(result_json, ostr);
            //     }
            //     else
            //     {
            //         response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
            //         response.setChunkedTransferEncoding(true);
            //         response.setContentType("application/json");
            //         std::ostream &ostr = response.send();

            //         Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            //         root->set("type", "/errors/not_found");
            //         root->set("title", "Internal exception");
            //         root->set("status", "404");
            //         root->set("detail", "user ot found");
            //         root->set("instance", "/user");
                    
            //         Poco::JSON::Stringifier::stringify(root, ostr);
            //     }
            //     return;
            // }

            
            // else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            // {
            //     if (form.has("first_name") && form.has("last_name") && form.has("email") && form.has("login") && form.has("password"))
            //     {
            //         database::User user;
            //         user.first_name() = form.get("first_name");
            //         user.last_name() = form.get("last_name");
            //         user.email() = form.get("email");
            //         user.title() = form.get("title");
            //         user.login() = form.get("login");
            //         user.password() = form.get("password");

            //         bool check_result = true;
            //         std::string message;
            //         std::string reason;

            //         if (!check_name(user.get_first_name(), reason))
            //         {
            //             check_result = false;
            //             message += reason;
            //             message += "<br>";
            //         }

            //         if (!check_name(user.get_last_name(), reason))
            //         {
            //             check_result = false;
            //             message += reason;
            //             message += "<br>";
            //         }

            //         if (!check_email(user.get_email(), reason))
            //         {
            //             check_result = false;
            //             message += reason;
            //             message += "<br>";
            //         }

            //         if (check_result)
            //         {
            //             user.save_to_mysql();
            //             response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            //             response.setChunkedTransferEncoding(true);
            //             response.setContentType("application/json");
            //             std::ostream &ostr = response.send();
            //             ostr << user.get_id();
            //             return;
            //         }
            //         else
            //         {
            //             response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
            //             std::ostream &ostr = response.send();
            //             ostr << message;
            //             response.send();
            //             return;
            //         }
            //     }
            // }
        }
        catch (...)
        {
        }

        response.setStatus(HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");

        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request ot found");
        root->set("instance", "/user");

        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }
private:
    class Checker {
    public:
        void Check(const database::User& user, std::string& message) {
            CheckName(user.GetFirstName());
            CheckName(user.GetLastName());
            CheckEmail(user.GetEmail());

            message = std::move(error_message_);
        }
    private:
        void AddError(const char* error) {
            error_message_ += error;
            error_message_ += "<br>";
        }

        void CheckName(const std::string &name) {
            if (name.length() < 3)
            {
                AddError("Name must be at least 3 signs");
                return;
            }
            
            if (name.find(' ') != std::string::npos)
            {
                AddError("Name can't contain spaces");
                return;
            }

            if (name.find('\t') != std::string::npos)
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

            if (email.find(' ') != std::string::npos)
            {
                AddError("Email can't contain spaces");
                return;
            }

            if (email.find('\t') != std::string::npos)
            {
                AddError("Email can't contain spaces");
            }
        }

        std::string error_message_;
    };

    std::string format_;
};
