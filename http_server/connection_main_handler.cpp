#include <errno.h>
#include <vector>
#include <string>
#include <stdint.h>

#include "connection_main_handler.h"
#include "http_utils.h"

using namespace std;

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
        if(strcmp((received_data + i), CRLF) == 0) {
            
            //advance the pointer
            i += strlen(CRLF) - 1;
            current_request += CRLF;

            //if request not empty, then add it to requests vector
            if(current_request.length() > 0) {
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
static vector<http_request_handler> handle_requests (const vector<string>& requests_string) {
    
    vector<http_request_handler> requests_handlers;
    
    for (const string& request : requests_string) {

        //do the conversion
        http_request_handler req_handler (request);

        //if this is a valid request, then add it to the vector
        if(req_handler) {
            requests_handlers.push_back(req_handler);
        } //else ignore
    }

    return requests_handlers;
}

/**
 * this function will be called from the pool-worker thread 
 * to handle the connection.
 */
void handle_connection(int sock_fd) {

    //buffer to store the request
    uint16_t buffer_length = 1024;
    uint8_t buffer[buffer_length];

    /**
     * this variable mark where the start of the writing to the buffer should occur.
     * this is important as the recv() function might return int the middle of the request,
     * in this case this request shouldn't be handled yet and another recv() operations
     * should occur until the full request is received.
     * so this variable keep track of how many bytes not yet handled in the buffer.
     */
    uint16_t buffer_start_index = 0;

    while(1) {

        //receive the request or multiple requests from the client
        ssize_t size = -1;
        uint16_t tries = 0;
        while(size != 0 && tries++ < MAX_RECV_TRIES) {
            ssize_t size = recv(new_fd, (buffer + buffer_start_index), buffer_length - buffer_start_index - 1, 0);
        }

        //server didn't receive any input for long period of time
        if(size == 0) {
            // TODO log reason of not receiving
            close(sock_fd);
            return;
        }

        //client closed the connection or error happened
        if(size == -1) {
            // TODO log the error with strerror(errno)
            close(sock_fd);
            return;
        }

        //total data in the buffer = the size of the current received data and the
        //the size of the previous not completed request.
        uint16_t total_buffered_data = size + buffer_start_index;

        //since HTTP is ASCII based protocol, we can use NULL byte to mark the end of string
        buffer[total_buffered_data] = '\0';

        //get the HTTP requests out of the received data
        vector<string> requests_string = sub_requests(buffer, total_buffered_data);

        //fix the start of the buffer and the buffer itself for the next read
        buffer_start_index = total_buffered_data - total_data_vector_length(requests_string);
        for(int i=0; i<buffer_start_index; i++) {
            buffer[i] = buffer[total_buffered_data - buffer_start_index + i];
        }

        //transform each valid string request to object
        vector<http_request_handler> requests_handlers = handle_requests(requests_string);

        //detect malformed requests
        if(requests_handlers.size() != requests_string.size()) {
            // TODO log the error as malformed request provided by the client.
            close(sock_fd);
            return;
        }

        //start executing the requests based on HTTP_VERSION
        //TODO
    }
}
