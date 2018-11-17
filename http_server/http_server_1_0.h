#ifndef HTTP_SERVER_1_0
#define HTTP_SERVER_1_0

#include <string>

#include "http_req_handler.h"
#include "http_responder.h"
#include "types_manager.h"
#include "http_server.h"
 /*
 * HTTP interface to be implemented by each HTTP handler.
 */
class http_server_v1_0 : public http_server{
	private:
		std::string post_url;
		int request_counter;
		int sock_fd;
		int expected_post;
		types_manager types_mng;
		http_responder responder;
	public:
		void handle_request(http_req_handler& request);
		void handle_data(const uint8_t* data, uint32_t data_length);
		bool is_end();
		int bypass_request_checking();
		http_server_v1_0(int sock_fd);
};

#endif //HTTP_SERVER_1_0
