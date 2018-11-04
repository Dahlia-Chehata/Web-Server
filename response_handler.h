#ifndef RESPONSE_HANDLER_H_
#define RESPONSE_HANDLER_H_

#include <string>
#include <map>

#include "http_utils.h"

class response_handler
{
private:
    // ************************** Status line **********************************
    HTTP_VERSION http_version;
    /**
     * indicating success or failure of the request. Common status codes are
     * 200, 404, or 302.
     */ 
    std::string status_code;
    /**
     * A brief, purely informational, textual description of the status code to
     * help a human understand the HTTP message.
     */ 
    std::string status_text;

    // ************************** Headers **************************************
    std::map<std::string, std::string> headers;

    // ************************** body *****************************************
    std::string body;
public:
    response_handler(/* args */);
    ~response_handler();

    void handle_response();

    void set_header(std::string key, std::string value);
    void set_body(std::string);
    void set_status_code(std::string);
    void set_status_text(std::string);
    void set_http_version(HTTP_VERSION);
};

#endif // RESPONSE_HANDLER_H_
