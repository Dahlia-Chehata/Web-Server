#ifndef YAWS_SERVER_H_
#define YAWS_SERVER_H_

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

#include "conn_pool.h"

#define PORT "7070"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define SERVER_ROOT "www" // where files will be served.


class yaws_server
{
private:
    size_t port;
	int sockfd; // the server will listen on this socket
    int new_fd; // new connection on new_fd
    conn_pool *pool;

    /**
     * client's address information
     * 
     * we use sockaddr_storage because it's large anough to hold both ipv4 and
     * ipv6 structures (struct sockaddr_in, struct sockaddr_in6)
     */
    struct sockaddr_storage client_addr;

    /**
     * Receive request from client connection.
     */ 
    int recv_request();
    /**
     * parse the received request using request_handler.
     */ 
    int parse_request();
    /**
     * after parsing the request this method will process the request
     * depending on the http method (post, get ..) using request_handler.
     */ 
    int process_request();
    /**
     * when processing request is done, repsonse_handler will prepare the
     * reponse.
     */ 
    int prepare_response();
    /**
     * when the response is ready send it.
     */ 
    int send_response();


public:
    yaws_server(/* args */);
    ~yaws_server();

    int init( void );
    int run( void );
};

#endif // YAWS_SERVER_H_
