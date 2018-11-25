#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "conn_main_handler.h"
#include "http_req_handler.h"
#include "http_utils.h"
#include "sock_RAII.h"
#include "atomic_RAII.h"
#include "http_server_1_0.h"
#include "http_server_1_1.h"
#include "http_server.h"
#include "http_responder.h"
#include "../logger.h"

using namespace std;

atomic<uint32_t> connections_number__;

/**
 * this function will take as a parameter the received data from
 * the client and try to divide it to multiple requests which
 * every request is terminated by CRLF.
 */
static vector<string> sub_requests (const uint8_t* received_data, uint16_t data_size) {

    //parse the received_data into several requests if found
    vector<string> requests;
    string current_request = "";

    for(int i=0; i<data_size; i++) {
    
        //check if in end of request
        if(strcmp((const char*)(received_data + i), CRLFCRLF) == 0) {
            
            //advance the pointer
            i += strlen(CRLFCRLF) - 1;
            current_request += CRLF;

            //if request not empty, then add it to requests vector
            if(current_request.length() > strlen(CRLF)) {
                requests.push_back(current_request);
                current_request = "";
            }//else, ignore
            
        } else {
            //add ASCII char to the request
            current_request += string(1, received_data[i]);
        }

    }

    return requests;
}

/**
 * this function return total length of every string in the provided vector.
 */
static uint32_t total_data_vector_length (const vector<string>& data_list) {

    uint32_t total_length = 0;

    for(const string& single_data : data_list) {
        total_length += single_data.length();
    }

    return total_length;
}

/**
 * this function convert a vector of requests represented as strings to 
 * vector of requests represented as http_request_handler objects.
 */
static vector<http_req_handler> handle_requests (const vector<string>& requests_string) {
    
    vector<http_req_handler> requests_handlers;
    
    for (const string& request : requests_string) {

        //do the conversion
        http_req_handler req_handler (request);

        //if this is a valid request, then add it to the vector
        if(req_handler) {
            requests_handlers.push_back(req_handler);
        } //else ignore
    }

    return requests_handlers;
}

static bool handle_post_request(int sock_fd, http_server* curr_server, uint32_t post_size) {

    //buffer to store post data
    unique_ptr<uint8_t[]> buffer(new uint8_t[post_size]);

    //fill the buffer
    uint32_t size = 0, tries = 0, total_received = 0;
    while(total_received != post_size && tries++ < MAX_RECV_TRIES) {

        size = recv(sock_fd, (buffer.get() + total_received), (post_size - total_received), 0);
        
        if(size == -1) {
            return false;
        }

        total_received += size;
    }

    //didn't receive the data for a long period of time
    if(total_received != post_size) {
        return false;
    }

    curr_server->handle_data(buffer.get(), total_received);
    return true;
}

/**
 * This function will be called from the pool-worker thread
 * to handle the connection.
 */
void handle_connection(int sock_fd) {

    //buffer to store the request
    uint32_t buffer_length = 1024;   //1-KB buffer
    unique_ptr<uint8_t[]> buffer (new uint8_t[buffer_length]);

    /**
     * this variable mark where the start of the writing to the buffer should occur.
     * this is important as the recv() function might return int the middle of the request,
     * in this case this request shouldn't be handled yet and another recv() operations
     * should occur until the full request is received.
     * so this variable keep track of how many bytes not yet handled in the buffer.
     */
    uint16_t buffer_start_index = 0;

    /**
     * resource acquisition is initialization
     * This used to auto-close the socked descriptor after this function goes out
     * of scope.
     */
    sock_RAII sock_prot(sock_fd);

    /**
     * responder to respond to the client if the http method not supported yet.
     */
    http_responder responder(sock_fd);

    /*
    * HTTP servers with different versions
    * will be listed here.
    */
    http_server* curr_server;
    http_server_v1_0 serv_1_0(sock_fd);
    http_server_v1_1 serv_1_1(sock_fd);

    /*
    * handle connection close.
    */
    time_t original_time;

    /* Obtain current time. */
    original_time = time(NULL);

    //handle number of connections
    atomic_RAII connections_number(connections_number__);

    //set timeout for socket operations
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    while(1) {

        //check timeout
        if(time(NULL) - original_time > CONNECTION_MAX_LIVE_TIME) {
            return;
        }

        //check congestion
        if(connections_number__ > NORMAL_CONNECTION_NUMBER &&
            time(NULL) - original_time > CONNECTION_MAX_CONGESTION_LIVE_TIME) {
            return;
        }

        //receive the request or multiple requests from the client
        ssize_t size = 0, tries = 0;
        while(size <= 0 && tries++ < MAX_RECV_TRIES) {
            size = recv(sock_fd, (buffer.get() + buffer_start_index), buffer_length - buffer_start_index - 1, 0);
        }

        //server didn't receive any input for long period of time
        if(size == 0) {
            // TODO log reason of not receiving
            return;
        }

        //client closed the connection or error happened
        if(size == -1) {
            // TODO log the error with strerror(errno)
            return;
        }

        //total data in the buffer = the size of the current received data and the
        //the size of the previous not completed request.
        uint16_t total_buffered_data = size + buffer_start_index;

        //since HTTP is ASCII based protocol, we can use NULL byte to mark the end of string
        buffer[total_buffered_data] = '\0';

        //get the HTTP requests out of the received data
        vector<string> requests_string = sub_requests(buffer.get(), total_buffered_data);

        yaws::log_new_request(requests_string);

        //fix the start of the buffer and the buffer itself for the next read
        buffer_start_index = total_buffered_data - total_data_vector_length(requests_string);
        for(int i=0; i<buffer_start_index; i++) {
            buffer[i] = buffer[total_buffered_data - buffer_start_index + i];
        }

        //transform each valid string request to object
        vector<http_req_handler> requests_handlers = handle_requests(requests_string);

        //detect malformed requests
        if(requests_handlers.size() != requests_string.size()) {
            // TODO log the error as malformed request provided by the client.
            return;
        }

        //start executing the requests based on HTTP_VERSION
        for(int i=0; i<requests_handlers.size(); i++) {

            if(requests_handlers[i].get_http_version() == HTTP_VERSION_HTTP1_0) {
                curr_server = &serv_1_0;
            }
            else if(requests_handlers[i].get_http_version() == HTTP_VERSION_HTTP1_1) {
                curr_server = &serv_1_1;
            }
            else {
                responder.set_http_version(HTTP_V1_1)
                ->set_http_statuscode("505 HTTP Version Not Supported")
                ->send_response();
                return;
            }

            curr_server->handle_request(requests_handlers[i]);

            //detect end of connection from http server
            if(curr_server->is_end()) {
                return;
            }

            //detect POST request
            if(curr_server->bypass_request_checking() > 0) {
                //try to handle the POST request, if failed, then return and end the connection.
                if(!handle_post_request(sock_fd, curr_server, curr_server->bypass_request_checking())) {
                    return;
                }
                //detect end of connection from http server
                if(curr_server->is_end()) {
                    return;
                }
            }
        }
    }
}
