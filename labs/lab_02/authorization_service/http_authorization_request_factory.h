#ifndef HTTP_AUTHORIZATION_REQUEST_FACTORY_H_HL_MAI
#define HTTP_AUTHORIZATION_REQUEST_FACTORY_H_HL_MAI

#include "authorization_handler.h"

#include <HLMAI/contains_substr.h>
#include <HLMAI/database/user/user_table_shard.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include <iostream>

class HTTPAutorizationRequestFactory :
    public Poco::Net::HTTPRequestHandlerFactory
{
 public:
  HTTPAutorizationRequestFactory(const std::string& format)
    : format_(format)
  {
    database::UserTableShard::GetInstance().Create();
  }

  Poco::Net::HTTPRequestHandler* createRequestHandler(
      const Poco::Net::HTTPServerRequest& request) override
  {
    return new AutorizationHandler(format_);
  }
 private:
  std::string format_;
};

#endif