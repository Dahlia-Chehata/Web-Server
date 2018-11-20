/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "client.h"
#include "../http_server/types_manager.h"
#include <iostream>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iterator>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>


#define MAXDATA 1024
#define PORT "80"

using namespace std;

bool rcv_ack_from_server (const int sockfd)
{
    char rcv_pkt[MAXDATA];
    recv(sockfd, rcv_pkt, MAXDATA, 0);
    cout << "ack " << rcv_pkt << endl;
    vector<string> response = split_cmd(rcv_pkt);
    return(response[1] == "200");
}
void send_file_to_server(const int sockfd,const string &filename)
{
    struct stat file_stat;
    char file_size[256];
    ssize_t bytes;
    off_t offset;
    int sent_bytes = 0;
    int remain_data;

    //establish the connection between file and file descriptor read only
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening file %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //fstat() stats the file pointed to by file descriptor fd and fills in buffer file_stat.
    if (fstat(fd, &file_stat) < 0) {
        fprintf(stderr, "Error fstat --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // get the file_stat size
    sprintf(file_size, "%jd", file_stat.st_size);

    // send file size
    bytes = send(sockfd, file_size, sizeof(file_size), 0);
    if (bytes < 0)
    {
        fprintf(stderr, "Error on sending file size %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    offset = 0;
    remain_data = file_stat.st_size;
    cout << "file size = " << file_size << endl;
//    while (((sent_bytes = sendfile(sockfd, fd, &offset, MAXDATA)) > 0) && (remain_data > 0)) {
//        remain_data -= sent_bytes;
//        cout << "remain data = " << remain_data << endl;
//    }

    while (((remain_data > MAXDATA) && (sent_bytes = sendfile(sockfd, fd, &offset, MAXDATA)) > 0)  ) {
      remain_data -= sent_bytes;
      cout << "remain data = " << remain_data << endl;
    }
    if (remain_data)
    {
        sendfile(sockfd, fd, &offset, remain_data);
        cout << "remain data = 0" << endl;
    }
}
void rcv_file_from_server(const int sockfd,const string &filename)
{
    char httpmsg[MAXDATA];
    FILE *received_file;
    int file_size, remain_data, bytes;

    // receive file size
    recv(sockfd, httpmsg, MAXDATA, 0);
    file_size=atoi(httpmsg);

    received_file = fopen(filename.c_str(), "w");
    if (received_file == NULL) {
        fprintf(stderr, "Failed to open file %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }

    remain_data = file_size;
    cout << "File size = " << file_size << endl;

    // receive file data
    while (remain_data > MAXDATA) {
        bytes = recv(sockfd, httpmsg, MAXDATA, 0);
        fwrite(httpmsg, sizeof(char), bytes, received_file);
        remain_data -= bytes;
        fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", bytes, remain_data);
    }

    while (remain_data > 0) {
        bytes = recv(sockfd, httpmsg, remain_data, 0);
        fwrite(httpmsg, sizeof(char), bytes, received_file);
        remain_data -= bytes;
        fprintf(stdout, " %d bytes received and %d bytes remaining\n", bytes, remain_data);

        if(bytes == 0) {
            cout << "Problem in receiving the rest of the data" << endl;
            break;
        }
    }

  fclose(received_file);
}
void send_header_line (const int fd,const string & method,const string &filename)
{
    types_manager tp;
    string msg = method + " " + filename + " HTTP/1.0\r\n";
    msg += "Content-Type: " + get_file_type(filename)+ "text/html\r\n";
    msg += "\r\n";
    //used when socket in a connected state
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
}
std::string get_file_type(std::string filename)
{
    if (filename.compare(filename.size() - 3, 3, "jpg") == 0) return "image/jpg";
    else if (filename.compare(filename.size() - 3, 4, "html") == 0) return "text/html";
    else return "text/plain";
}
int main(int argc, char *argv[]) {
    string command;
    int sockfd, status;
    struct addrinfo hints;
    struct addrinfo *res;

    // Open the TCPConnection
    if (argc < 2) {
        fprintf(stderr,"Insufficient parameters\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char * port_number =(char*) PORT;
    if(argc==3)
    {
        port_number=argv[2];
    }

    // getaddrinfo() system call
    if ((status = getaddrinfo(argv[1], port_number, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // socket() system call
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("client: socket");
        exit(1);
    }

    clock_t begin = clock();


    // Process the commands
    ifstream commandsfile ("commands.txt", ios::in);
    if(commandsfile.is_open()) {
        while(getline(commandsfile, command)) {
            vector<string> splitted_cmd = split_cmd(command);
            send_header_line(sockfd, splitted_cmd[0], splitted_cmd[1]);
            bool positive_ack = rcv_ack_from_server(sockfd);
            if (splitted_cmd[0] == "GET") {
                if(positive_ack) {
                    rcv_file_from_server(sockfd, splitted_cmd[1]);
                } else {
                    cout << "Error 404 file not found" << endl;
                }
            } else if (splitted_cmd[0] == "POST") {
                if(positive_ack) {
                    send_file_to_server(sockfd, splitted_cmd[1]);
                } else {
                    cout << "Error in POST of file " << splitted_cmd[1] << endl;
                }
            }
        }
        commandsfile.close();
    }

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "-------------------------------------------" << endl;
    cout << "\nElapsed time = " << elapsed_secs << " secs\n" << endl;
    cout << "-------------------------------------------" << endl;

    // Close the TCPConnection
    freeaddrinfo(res);
    shutdown(sockfd, 2);
    return 0;
}
