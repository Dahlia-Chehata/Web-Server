#ifndef HTTP_REQUEST_HANDLER_H_
#define HTTP_REQUEST_HANDLER_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "http_utils.h"
#include "../utils/string_utils.h"

class http_req_handler
{
private:
    
    // parsed first line of the request
    HTTP_METHOD http_method;
    HTTP_VERSION http_version;
    std::string target;
    std::string request;
    bool valid;

    // the rest of the headers
    std::unordered_map<std::string, std::string> headers;

    /**
     * This parse the request and set the values of the fields.
     */
    void parse_request();
    HTTP_METHOD get_http_method_pv(const std::string& method_str);
    HTTP_VERSION get_http_version_pv(const std::string& version_str);

public:

    http_req_handler(const std::string& request);
    ~http_req_handler();
    HTTP_METHOD get_http_method();
    HTTP_VERSION get_http_version();
    std::string get_http_url_target();
    std::unordered_map<std::string, std::string> get_headers();
    std::string get_header_value(const std::string& header_type);

    /**
     * to print request to console.
     * implicit conversion.
     */ 
    operator std::string () const {return request;}

    /**
     * to check if the request valid.
     */ 
    operator bool () const {return valid;}
};

#endif // HTTP_REQUEST_HANDLER_H_
