#ifndef YAWS_LOGGER_H_
#define YAWS_LOGGER_H_

#include <string>

namespace yaws
{
    /**
     * 
     * This function will print the welcome message when the server is
     * successfully running.
     * 
     */ 
    void print_welcome_message()
    {
        // TODO
    }

    /**
     * 
     * This function should log the incoming requests to the server.
     * 
     * log format:
     *  [IP HOST] -=- [dd/mm/yyy hh:mm:ss] "[REQUEST START LINE]" [RESPONSE STATUS CODE]
     * 
     * example:
     *  127.0.0.1 -=- [24/Feb/2014 10:26:28] "GET / HTTP/1.1" 200
     * 
     */ 
    void log(std::string start_line, std::string res_code)
    {
        // TODO
    }
}

#endif // YAWS_LOGGER_H_
