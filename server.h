#ifndef YAWS_SERVER_H
#define YAWS_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT "7070"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

namespace yaws
{
    int start_server( void );
}

#endif // YAWS_SERVER_H
