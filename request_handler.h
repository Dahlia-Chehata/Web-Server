#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include <string>
#include <map>

#include "http_utils.h"

class request_handler
{
private:
    // *************************** Start line **********************************
    HTTP_METHOD http_method;
    HTTP_VERSION http_version;
    std::string target;
    
    // *************************** Headers *************************************
    std::string content_type;
    std::string host_name;
	std::string user_agent;
    std::map<std::string, std::string> headers;

    // *************************** Body ****************************************
    std::string body;
public:
    request_handler(/* args */);
    ~request_handler();

    /**
     * This parse the request and set the values of the fields.
     */
    void parse_request();

    std::string get_content_type();
    std::string get_body();

    /**
     * to print request to console.
     */ 
    void to_string();
};

#endif // REQUEST_HANDLER_H_
