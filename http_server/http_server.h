#ifndef HTTP_SERVER_
#define HTTP_SERVER_

#include "http_req_handler.h"

 /*
 * HTTP interface to be implemented by each HTTP handler.
 */
class http_server{
	public:
		void handle_request(const http_req_handler& request);
		void handle_data(const std::string& data);
		bool is_end();
		int bypass_request_checking();
};

#endif //HTTP_SERVER_
