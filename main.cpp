#include "yaws_server.h"

int main(int argc, char* argv[])
{
    // start the server
    yaws_server* http_server = new yaws_server();
    
    http_server->run();

    // do we really need this? the call to run will never return aslun right?
    free(http_server);

    return 0;
}
