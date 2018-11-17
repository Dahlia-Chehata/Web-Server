#ifndef HTTP_SERVER_
#define HTTP_SERVER_

#include "http_req_handler.h"

 /*
 * HTTP interface to be implemented by each HTTP handler.
 */
class http_server{
	public:
		http_server() = default;
		virtual ~http_server() {};
		virtual void handle_request(http_req_handler& request) = 0;
		virtual void handle_data(const uint8_t* data, uint32_t data_length) = 0;
		virtual bool is_end() = 0;
		virtual int bypass_request_checking() = 0;
};

#endif //HTTP_SERVER_
