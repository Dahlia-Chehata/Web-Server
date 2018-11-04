#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

class request_handler
{
private:
    char* request;
public:
    request_handler(/* args */) { }
    ~request_handler() { }

    void parse_request();
};

#endif // REQUEST_HANDLER_H_
