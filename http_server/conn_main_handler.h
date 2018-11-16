#ifndef CONNECTION_MAIN_HANDLER_H_
#define CONNECTION_MAIN_HANDLER_H_

#define MAX_RECV_TRIES 5

/**
 * this function will be called from the pool-worker thread 
 * to handle everything related to the connection.
 */
void handle_connection(int sock_fd);


#endif // CONNECTION_MAIN_HANDLER_H_
