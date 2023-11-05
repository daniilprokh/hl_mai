#include "http_group_chat_request_factory.h"

#include <HLMAI/http_web_server.h>

int main(int argc, char *argv[]) {
  HTTPWebServer<HTTPGroupChatRequestFactory> app;
  return app.run(argc, argv);
}
