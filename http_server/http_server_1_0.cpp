#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "http_req_handler.h"

using namespace std;


static vector<string> split_text (string text, const string& delimiter) {

    size_t pos = 0;
    string token;
    vector<string> splited_text;

    while ((pos = text.find(delimiter)) != string::npos) {
        token = text.substr(0, pos);
        if(token.length() > 0) {
            splited_text.push_back(token);
        }
        text.erase(0, pos + delimiter.length());
    }

    if(text.length() > 0) {
        splited_text.push_back(text);
    }

    return splited_text;
}

http_server_v1_0::http_server_v1_0(int sock_fd) {
	this->sock_fd = sock_fd;
	request_counter = 0;
	expected_post = 0;
}

void http_server_v1_0::handle_request(const http_req_handler& request) {

	//increment request number
	request_counter++;

	//check the method if it was supported or not
	if(request.get_http_method() == HTTP_METHOD_NOT_IMPLEMENTED) {
		//TODO respond with 400 bad request
		return;
	}

	//handle get requests
	if(request.get_http_method() == HTTP_METHOD_GET) {

		//create path to the file
		string base_dir = "./web"
		string file_path = base_dir + last_request.get_http_url_target();

		//handle index file
		if(file_path == base_dir + "/") {
			file_path += "index.html";
		}

		//try to open the file if existed
		FILE * fp;
		if((fp = fopen(file_path, "r")) == NULL) {
			//TODO respond with 404 not found
			return;
		}

		//get it's dimensions
    	struct stat filestat;
    	int fd;
    	if (((fd = open (filename, O_RDONLY)) < 0) || (fstat(fd, &filestat) < 0)) {
    		close(fd);
			//TODO respond with 500 server internal error
			return;
    	}
    	close(fd);

		//TODO respond with 200 ok
		//TODO respond with content-length = filestat.st_size
		//TODO respond with appropriate header depends on the file type
		//TODO respond with file data
	}

	//handle post requests
	if(request.get_http_method() == HTTP_METHOD_POST) {

		//the length of the received file
		if(request.get_header_value("Content-length") == "") {
			//TODO respond with 400 bad request
			return;
		}

		expected_post = stoi(request.get_header_value("Content-length"));
		//TODO limit the posted file
		//TODO check the type of the uploaded file
		//TODO respond with 200 ok
	}

}

void http_server_v1_0::handle_data(const std::string& data)  {

	//increment request number
	request_counter++;

	//server give data less than wanted
	if(expected_post != data.length() || !expected_post) {
		//TODO respond with 500 internal error
		return;
	}

	//get the full path to the file. all are directories by the last is file
	vector<string> path = split_text(last_request.get_http_url_target(), "/");

	//build directories
	string base_dir = "./web/"
	for(int i=0; i<path.length()-1; i++) {
		base_dir += path[i];
		//check if error while creating the directory
		if(mkdir(base_dir, 0700) == -1) {
			//TODO respond with 500 internal error
			return;
		}
		base_dir += "/";
	}

	//create the file
	path += path.back();
	FILE * fp = fopen(path, "a");
	//check if error while creating the file
	if(fp == NULL) {
		//TODO respond with 500 internal error
		return;
	}

	//write data to the file
	fputs(data.c_str(), fp);
	fclose(fp);

	//set the new http_v1.0 state
	expected_post = 0;
}

bool http_server_v1_0::is_end() {
	return !(request_counter == 0 || (request_counter == 1 && expected_post > 0));
}

int http_server_v1_0::bypass_request_checking() {
	return expected_post;
}
