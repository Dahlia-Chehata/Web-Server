#ifndef HTTP_UTILS_H_
#define HTTP_UTILS_H_

#define	CR	  '\r'    // Carriage Return
#define LF	  '\n'    // Line Feed
#define	CRLF  "\r\n"

enum HTTP_METHOD
{
    GET,
    POST,
    // PUT,
    // HEAD,
    NOT_IMPLEMENTED
};

enum HTTP_PROTOCOL {
    HTTP1_0,
    HTTP1_1,
    HTTP_UNSUPPORTED
};

#endif // HTTP_UTILS_H_
