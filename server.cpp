#include "server.h"

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int yaws::start_server(void)
{
    int sockfd; // the server will listen on this socket
    int new_fd; // new connection on new_fd

    struct addrinfo hints, *servinfo; // *serveinfo is pointer to a linked list
    struct addrinfo *p; // *p used for looping over the serveinfo

    /**
     * client's address information
     * 
     * we use sockaddr_storage because it's large anough to hold both ipv4 and
     * ipv6 structures (struct sockaddr_in, struct sockaddr_in6)
     */
    struct sockaddr_storage client_addr;

    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1; // passed to setsockopt
    
    /**
     * The client ip address in printable format
     * We use INET6_ADDRSTRLEN so we have space for both ipv4 and ipv6 address
     * length.
     */ 
    char client_ip_address[INET6_ADDRSTRLEN];
    int status; // return value of getaddrinfo() system call.

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // this will use local host IP

    /**
     * getaddrinfo() system call helps set up and fills structs we need later in
     * calling other system calls.
     * it also do DNS and service name lookups. super.
     */ 
    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, 
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue; // continue to the next result.
        }

        /**
         * So we can resuse the socket again if we try to re-run the server but
         * the socket is still hanging aroudn in the kernel.
         */ 
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        /**
         * Since this is a server we need to associate it with a port number, a
         * port number is used by the kernel to match an incoming packet to a
         * certain process's socket descriptor.
         * 
         * A port number is used by clients to connect to the server.
         */
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd); // socket should be closed.

            perror("server: bind");
            continue; // continue to the next result.
        }

        break; // socket successfully associated with a port on local host.
    }

    freeaddrinfo(servinfo); // all done with this structure, RIP.

    // if all results failed to bind.
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(EXIT_FAILURE);
    }

    /**
     * listen and wait for incoming connections.
     * 
     * BACKLOG is the number of connections allowed in the incoming queue.
     * any new connection will wait in the queue until we accept() it.
     */
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("server: waiting for connections...\n");

    // wait and accept incoming connections.
    // main server loop.
    while(1) {
        sin_size = sizeof client_addr;
        /**
         * Accept will return new socket file descriptor to use for this single
         * connection, so we can use recv() and send().
         */
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        /**
         * ntop: network to printable/presentation, this convert the binary
         * representation of IP address to readable string
         * (numbers-and-dots notation).
         */
        inet_ntop(
            client_addr.ss_family,
            get_in_addr((struct sockaddr *)&client_addr),
            client_ip_address,
            sizeof client_ip_address
        );

        printf("server: got connection from %s\n", client_ip_address);

        /**
         * to handle multiple clients we use fork.
         * when there is new connection, we accept it and fork a child process
         * to handle it.
         */
        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1) {
                perror("send");
            }

            close(new_fd);
            exit(EXIT_SUCCESS);
        }
        close(new_fd);  // parent doesn't need this
    }
    
    // never return
    return 0;
}
