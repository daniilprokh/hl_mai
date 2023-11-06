#include "http_other_request_factory.h"

#include <HLMAI/http_web_server.h>

int main(int argc, char*argv[]) 
{
  HTTPWebServer<HTTPOtherRequestFactory> app;
  return app.run(argc, argv);
}
