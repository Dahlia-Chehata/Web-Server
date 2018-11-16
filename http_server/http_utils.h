#ifndef HTTP_UTILS_H_
#define HTTP_UTILS_H_

#define	CR	  '\r'    // Carriage Return
#define LF	  '\n'    // Line Feed
#define	CRLF  "\r\n"
#define	CRLFCRLF  "\r\n\r\n"

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

#endif // HTTP_UTILS_H_
