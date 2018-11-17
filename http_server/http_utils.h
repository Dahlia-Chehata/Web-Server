#ifndef HTTP_UTILS_H_
#define HTTP_UTILS_H_

#define	CR	  '\r'    // Carriage Return
#define LF	  '\n'    // Line Feed
#define	CRLF  "\r\n"
#define	CRLFCRLF  "\r\n\r\n"

#define HTTP_V1_0 "HTTP/1.0"
#define HTTP_V1_1 "HTTP/1.1"

#include <string>
#include <vector>

enum HTTP_METHOD
{
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_NOT_IMPLEMENTED
};

enum HTTP_VERSION {
    HTTP_VERSION_HTTP1_0,
    HTTP_VERSION_HTTP1_1,
    HTTP_VERSION_HTTP_UNSUPPORTED
};

std::vector<std::string> split_text (std::string text, const std::string& delimiter);

#endif // HTTP_UTILS_H_
