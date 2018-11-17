#ifndef HTTP_SERVER_1_0
#define HTTP_SERVER_1_0

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "http_req_handler.h"
#include "http_responder.h"
#include "types_manager.h"
#include "http_utils.h"
#include "http_server.h"


 /*
 * HTTP/1.0 class to handle request to thing version of the protocol.
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
