#ifndef YAWS_LOGGER_H_
#define YAWS_LOGGER_H_

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "utils/string_utils.h"

namespace yaws
{
    /**
     * 
     * This function will print the welcome message when the server is
     * successfully running.
     * 
     */ 
    static void print_welcome_message()
    {
        std::cout << "+--------------------------------+" << std::endl;
        std::cout << "|                                |" << std::endl;
        std::cout << "| WELCOME TO YAWS SERVER (v0.1)  |" << std::endl;
        std::cout << "|                                |" << std::endl;
        std::cout << "+--------------------------------+" << std::endl;
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
    static void log_new_request(const std::vector<std::string>& requests)
    {
        // auto time_now = std::chrono::system_clock::now();
        // std::cout << time_now << std::endl;

        for (const std::string& req : requests)
        {
            std::cout << yaws::split_text(req, "\n").front() << std::endl;
        }
    }

    static void log_new_connection(char* cline_ip)
    {
        std::cout << "NEW CONNECTION FROM: " << cline_ip << std::endl;
    }
}

#endif // YAWS_LOGGER_H_
