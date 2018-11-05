#ifndef HTTP_REQUEST_HANDLER_H_
#define HTTP_REQUEST_HANDLER_H_

#include <string>
#include <unordered_map>

#include "http_utils.h"

class http_request_handler
{
private:
    
    // parsed first line of the request
    HTTP_METHOD http_method;
    HTTP_VERSION http_version;
    std::string target;

    // the rest of the headers
    std::unordered_map<std::string, std::string> headers;

    /**
     * This parse the request and set the values of the fields.
     */
    void parse_request(const std::string& request);

public:

    request_handler(const std::string& request);
    ~request_handler();

    HTTP_METHOD get_http_method();
    HTTP_VERSION get_http_version();
    std::string get_http_url_target();
    std::unordered_map<std::string, std::string> get_headers();
    std::string get_header_value(const std::string& header_type);

    /**
     * to print request to console.
     * implicit conversion.
     */ 
    operator std::string () const;

    /**
     * to check if the request valid.
     */ 
    operator bool () const;
};

#endif // HTTP_REQUEST_HANDLER_H_
