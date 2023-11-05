#ifndef HTTP_P2P_CHAT_REQUEST_FACTORY_H_HL_MAI
#define HTTP_P2P_CHAT_REQUEST_FACTORY_H_HL_MAI

#include "p2p_chat_handler.h"

#include <HLMAI/contains_substr.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>

#include <iostream>

class HTTPP2pChatRequestFactory :
    public Poco::Net::HTTPRequestHandlerFactory
{
 public:
  HTTPP2pChatRequestFactory(const std::string &format)
    : format_(format)
  {
    database::P2pChatTable::GetInstance().Create();
    database::P2pChatMessageTable::GetInstance().Create();
  }

  Poco::Net::HTTPRequestHandler* createRequestHandler(
      const Poco::Net::HTTPServerRequest& request) override
  {
    const std::string& uri = request.getURI();
    std::cout << "request: " << uri << std::endl;
    return new P2pChatHandler(format_);
  }
 private:
  std::string format_;
};

#endif
