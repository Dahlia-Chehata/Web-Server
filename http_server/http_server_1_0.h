#ifndef HTTP_SERVER_1_0
#define HTTP_SERVER_1_0

#include "http_req_handler.h"
#include "types_manager.h"

 /*
 * HTTP interface to be implemented by each HTTP handler.
 */
class http_server_v1_0 : public http_server{
	private:
		http_req_handler last_request;
		int request_counter;
		int sock_fd;
		bool expected_post;
		types_manager types_mng;
	public:
		http_server_v1_0(int sock_fd);
};

#endif //HTTP_SERVER_1_0
