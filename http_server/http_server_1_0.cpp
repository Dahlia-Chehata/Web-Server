#include "http_server_1_0.h"

using namespace std;

http_server_v1_0::http_server_v1_0(int sock_fd) : responder(sock_fd), http_server() {
	this->sock_fd = sock_fd;
	request_counter = 0;
	expected_post = 0;
	post_url = "";
}

void http_server_v1_0::handle_request(http_req_handler& request) {

	//increment request number
	request_counter++;

	//check the method if it was supported or not
	if(request.get_http_method() == HTTP_METHOD_NOT_IMPLEMENTED) {
		responder.set_http_version(HTTP_V1_0)
		->set_http_statuscode("405 Method Not Allowed")
		->send_response();
		return;
	}

	//handle get requests
	if(request.get_http_method() == HTTP_METHOD_GET) {

		//create path to the file
		string base_dir = "./web";
		string file_path = base_dir + request.get_http_url_target();

		//handle index files
		if(file_path[file_path.length()-1] == '/') {
			file_path += "index.html";
		}

		FILE * fp;
		//check if supported file first.
		if(types_mng.is_file_supported(file_path)) {
			//try to open the file if existed
			if((fp = fopen(file_path.c_str(), "r")) == NULL) {	
				responder.set_http_version(HTTP_V1_0)
				->set_http_statuscode("404 Not Found")
				->send_response();
				return;
			}
		} else {
			//treat it as directory and try again
			string new_file_path = file_path + "/index.html";
			if((fp = fopen(new_file_path.c_str(), "r")) == NULL) {
				responder.set_http_version(HTTP_V1_0)
				->set_http_statuscode("404 Not Found")
				->send_response();
				return;
			//if it's a directory, then redirect the user to the dir url
			} else {
				fclose(fp);
				responder.set_http_version(HTTP_V1_0)
				->set_http_statuscode("301 Moved Permanently")
				->add_header("Location: " + request.get_http_url_target() + "/")
				->send_response();
				return;
			}
		}

		//get it's dimensions
    	struct stat filestat;
    	int fd;
    	if (((fd = open (file_path.c_str(), O_RDONLY)) < 0) || (fstat(fd, &filestat) < 0)) {
    		close(fd);
			fclose(fp);
			responder.set_http_version(HTTP_V1_0)
			->set_http_statuscode("500 Internal Server Error")
			->send_response();
			return;
    	}
    	close(fd);

		char content_length_buffer[24];

		//read the data from the file
		char* data_buffer = (char*) malloc(filestat.st_size+1);
		if(data_buffer == NULL) {
			responder.set_http_version(HTTP_V1_0)
			->set_http_statuscode("500 Internal Server Error")
			->send_response();
			fclose(fp);
			return;
		}
		if(fread(data_buffer, filestat.st_size, 1, fp) == filestat.st_size) {
			responder.set_http_version(HTTP_V1_0)
			->set_http_statuscode("500 Internal Server Error")
			->send_response();
			free(data_buffer);
			fclose(fp);
			return;
		}
		data_buffer[filestat.st_size] = '\0';

		stringstream integer_converter;
		integer_converter << filestat.st_size;

		responder.set_http_version(HTTP_V1_0)
		->set_http_statuscode("200 OK")
		->add_header("Content-Length: " + integer_converter.str())
		->add_header(types_mng.generate_file_header(file_path))
		->set_data((const uint8_t *) data_buffer, (int)filestat.st_size)
		->send_response();

		free(data_buffer);
		fclose(fp);
		return;
	}

	//handle post requests
	if(request.get_http_method() == HTTP_METHOD_POST) {

		//the length of the received file
		if(request.get_header_value("Content-Length") == "") {
			responder.set_http_version(HTTP_V1_0)
			->set_http_statuscode("411 Length Required")
			->send_response();
			return;
		}

		if(!types_mng.is_file_supported(request.get_http_url_target())) {
			responder.set_http_version(HTTP_V1_0)
			->set_http_statuscode("415 Unsupported Media Type")
			->send_response();
			return;
		}

		//try to get the content length
		char* pEnd;
		expected_post = strtol (request.get_header_value("Content-Length").c_str(),&pEnd,10);
		post_url = request.get_http_url_target();
		//TODO limit the posted file //413 Payload Too Large
		responder.set_http_version(HTTP_V1_0)
		->set_http_statuscode("200 OK")
		->send_response();
	}

}

void http_server_v1_0::handle_data(const uint8_t* data, uint32_t data_length)  {

	//increment request number
	request_counter++;

	//server give data less than wanted
	if(expected_post != data_length || !expected_post) {
		responder.set_http_version(HTTP_V1_0)
		->set_http_statuscode("500 Internal Server Error")
		->send_response();
		return;
	}

	//get the full path to the file. all are directories by the last is file
	vector<string> path = yaws::split_text(post_url, "/");

	//build directories
	string base_dir = "./web/";
	for(int i=0; i<path.size()-1; i++) {
		base_dir += path[i] + "/";
		//check if dir already there
		DIR* dir = opendir(base_dir.c_str());
		if (dir) {
		    /* Directory exists*/
		    closedir(dir);
		    continue;
		}
		//check if error while creating the directory
		if(mkdir(base_dir.c_str(), 0700) == -1) {
			//TODO respond with 500 internal error
			return;
		}
	}

	//create the file
	base_dir += path.back();
	FILE * fp = fopen(base_dir.c_str(), "w");
	//check if error while creating the file
	if(fp == NULL) {
		responder.set_http_version(HTTP_V1_0)
		->set_http_statuscode("500 Internal Server Error")
		->send_response();
		return;
	}

	//write data to the file
	fwrite(data, data_length, 1, fp);
	fclose(fp);

	//set the new http_v1.0 state
	expected_post = 0;

	//send last 200Ok
	responder.set_http_version(HTTP_V1_0)
	->set_http_statuscode("200 OK")
	->send_response();
	return;
}

bool http_server_v1_0::is_end() {
	return !(request_counter == 0 || (request_counter == 1 && expected_post > 0));
}

int http_server_v1_0::bypass_request_checking() {
	return expected_post;
}
