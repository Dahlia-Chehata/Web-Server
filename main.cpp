#include "yaws_server.h"

int main(int argc, char* argv[])
{
    // start the server
    yaws_server my_server(argv);
    my_server.run();

    return 0;
}
