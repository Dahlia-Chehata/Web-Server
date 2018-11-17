#ifndef HTTP_SERVER_1_1
#define HTTP_SERVER_1_1

#include <string>

#include "http_req_handler.h"
#include "http_responder.h"
#include "types_manager.h"
#include "http_server.h"

 /*
 * HTTP/1.1 class to handle request to thing version of the protocol.
 */

class http_server_v1_1 : public http_server{
	private:
		std::string post_url;
		int request_counter;
		int sock_fd;
		int expected_post;
		bool terminated;
		types_manager types_mng;
		http_responder responder;
	public:
		void handle_request(http_req_handler& request);
		void handle_data(const uint8_t* data, uint32_t data_length);
		bool is_end();
		int bypass_request_checking();
		http_server_v1_1(int sock_fd);
};

#endif //HTTP_SERVER_1_1
