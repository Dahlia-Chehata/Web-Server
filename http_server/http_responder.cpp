#include <vector>
#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "http_utils.h"
#include "http_responder.h"

using namespace std;


bool http_responder::totally_send(const char* data, int data_size) {

    int total = 0;        // how many bytes we've sent
    int bytes_left = data_size; // how many we have left to send
    int n;

    while(total < data_size) {
        n = send(sock_fd, data+total, bytes_left, 0);
        if (n == -1) { break; }
        total += n;
        bytes_left -= n;
    }

    return n==-1?false:true; // return -1 on failure, 0 on success

	}

void http_responder::initialize_data() {
	headers.clear();
	http_version = "";
	status_code = "";
	data = "";
	if(data_ != NULL) {
		free(data_);
	}
	data_ = NULL;
	valid = true;
	data_size_ = 0;
}

http_responder::http_responder(int sock_fd) {
	data_ = NULL;
	initialize_data();
	this->sock_fd = sock_fd;
}

http_responder::~http_responder() {
	if(data_ != NULL) {
		free(data_);
	}
}

http_responder* http_responder::set_http_version (const std::string& http_version) {
	this->http_version = http_version;
	return this;
}

http_responder* http_responder::set_http_statuscode (const std::string& status_code) {
	this->status_code = status_code;
	return this;
}

http_responder* http_responder::add_header (const std::string& header) {
	headers.push_back(header + CRLF);
	return this;
}

http_responder* http_responder::set_data (const std::string& data) {
	this->data = data + CRLF;
	return this;
}

http_responder* http_responder::set_data (const uint8_t* data, int size) {

	//copy the data locally
	this->data_ = (char*) malloc(size);
	if(this->data_ != NULL) {
		data_size_ = size;
		memcpy(data_, data, size);
	}
	else {
		//can't malloc that amount of data
		valid = false;
	}
	return this;
}

bool http_responder::send_response() {

	/* some checking */
	if(valid == false) {
		initialize_data();
		return false;
	}

	if(http_version == "") {
		initialize_data();
		return false;
	}

	if(status_code == "") {
		initialize_data();
		return false;
	}

	/* send the data */

	//send the head line
	string head = http_version + " " + status_code + CRLF;
	totally_send(head.c_str(), head.length());

	//add content-length header
	if(data_ == NULL && data == "") {
		headers.push_back(string("Content-Length: 0") + CRLF);
	}

	//add status code header
	headers.push_back("Status: " + status_code + CRLF);

	//send the headers
	for(const auto& header: headers) {
		totally_send(header.c_str(), header.length());
	}


	//end the header
	totally_send(CRLF, strlen(CRLF));

	//send the data
	if(data_ != NULL) {
		totally_send(data_, data_size_);
	}
	else if(data != "") {
		totally_send(data.c_str(), data.length());
	}

	initialize_data();
	return true;
}
