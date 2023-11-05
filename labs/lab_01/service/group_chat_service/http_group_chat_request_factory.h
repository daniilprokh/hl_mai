#ifndef HTTP_GROUP_CHAT_REQUEST_FACTORY_H_HL_MAI
#define HTTP_GROUP_CHAT_REQUEST_FACTORY_H_HL_MAI

#include "group_chat_handler.h"

//#include <HLMAI/database/group_chat/group_chat_table.h>
#include <HLMAI/contains_substr.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include <iostream>

class HTTPGroupChatRequestFactory :
    public Poco::Net::HTTPRequestHandlerFactory
{
 public:
  HTTPGroupChatRequestFactory(const std::string &format)
    : format_(format)
  {
    database::ChatUserTable::GetInstance().Create();
    database::GroupChatTable::GetInstance().Create();
    database::GroupChatMessageTable::GetInstance().Create();
  }

  Poco::Net::HTTPRequestHandler* createRequestHandler(
      const Poco::Net::HTTPServerRequest& request) override
  {
    const std::string& uri = request.getURI();
    std::cout << "request: " << uri << std::endl;
    return new GroupChatHandler(format_);
  }
 private:
  std::string format_;
};

#endif
