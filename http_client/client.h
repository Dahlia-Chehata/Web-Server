/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   client.h
 * Author: Dahlia
 *
 * Created on November 16, 2018, 2:38 AM
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sys/socket.h>
#define MAXDATA 100


/**
 * split the commands : GET / POST
 * @param command
 * @return
 */
std::vector <std::string> split_cmd(std::string command)
{
  std::stringstream ss(command);
  std::istream_iterator<std::string> begin(ss),end;
  return  std::vector<std::string> (begin, end);
}
/**
 * to get the Content-Length of file required for POST request
 * @param filename
 * @return
 */
std::ifstream::pos_type file_size(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}
/**
 * receive acknowledgement from server to initiate data transport
 * @param sockfd
 * @return
 */
bool rcv_ack_from_server (const int sockfd);
/**
 * receive file from server in case of GET requet
 * @param sockfd
 * @param filename
 */
void rcv_file_from_server(const int sockfd,std::string &filename);
/**
 * receive file from server in case of POSt method
 * @param sockfd
 * @param filename
 */
void send_file_to_server(const int sockfd,const std::string &filename);
/**
 * send header to server before acknowledgment reception
 * @param fd
 * @param method
 * @param filename
 */
void send_header_line(const int fd,const std::string &method, std::string &filename);

#endif /* CLIENT_H */

