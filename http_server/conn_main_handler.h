#ifndef CONNECTION_MAIN_HANDLER_H_
#define CONNECTION_MAIN_HANDLER_H_

#define MAX_RECV_TRIES 5
#define CONNECTION_MAX_LIVE_TIME 15
#define CONNECTION_MAX_CONGESTION_LIVE_TIME 5
#define NORMAL_CONNECTION_NUMBER 100


/**
 * this function will be called from the pool-worker thread 
 * to handle everything related to the connection.
 */
void handle_connection(int sock_fd);


#endif // CONNECTION_MAIN_HANDLER_H_
