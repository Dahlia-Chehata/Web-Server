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
bool rcv_ack_from_server (int sockfd);
void rcv_file_from_server(int sockfd,std::string filename);
void send_file_to_server();
void send_header_line(int fd,std::string method, std::string filename);
std::string get_file_type(std::string file_name);

#endif /* CLIENT_H */

