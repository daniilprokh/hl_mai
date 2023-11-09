#include "http_authorization_request_factory.h"

#include <HLMAI/http_web_server.h>

int main(int argc, char *argv[]) {
  HTTPWebServer<HTTPAutorizationRequestFactory> app;
  return app.run(argc, argv);
}