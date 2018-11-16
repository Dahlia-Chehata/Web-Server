#ifndef HTTP_RESPONDER_
#define HTTP_RESPONDER_

#include <vector>
#include <string>

 /*
 * the class is responsible of generating http responses
 * with status code, http version, headers and data.
 */
class http_responder{
private:
	std::vector<std::string> headers;
	std::string http_version;
	std::string status_code;
	std::string data;
	int data_size_;
	int sock_fd;
	char* data_;
	bool valid;

	void initialize_data();
	bool totally_send(const char* data, int data_size);
public:
	http_responder(int sock_fd);
	~http_responder();
	http_responder* set_http_version (const std::string& http_version);
	http_responder* set_http_statuscode (const std::string& status_code);
	http_responder* add_header (const std::string& header);
	http_responder* set_data (const std::string& data);
	http_responder* set_data (const uint8_t* data, int size);
	bool send_response();
};

#endif //HTTP_RESPONDER_
