/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   client.h
 * Author: lenovo
 *
 * Created on November 16, 2018, 2:38 AM
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <sstream>
#include <vector>
#include <iterator>

std::vector <std::string> split_cmd(std::string command)
{
  std::stringstream ss(command);
  std::istream_iterator<std::string> begin(ss),end;
  return  std::vector<std::string> (begin, end);
}
bool rcv_ack_from_server (const int sockfd);
void rcv_file_from_server(const int sockfd,const std::string &filename);
void send_file_to_server();
void send_header_line(const int fd,const std::string &method,const std::string &filename);
std::string get_file_type(std::string file_name);

#endif /* CLIENT_H */

