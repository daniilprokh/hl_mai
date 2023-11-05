#include "http_p2p_chat_request_factory.h"

#include <HLMAI/http_web_server.h>

int main(int argc, char *argv[]) {
  HTTPWebServer<HTTPP2pChatRequestFactory> app;
  return app.run(argc, argv);
}
