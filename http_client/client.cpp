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
#include <stdio.h>
#include <vector>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iterator>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zconf.h>
#include <unistd.h>

#if defined(__linux__)
    #include <sys/sendfile.h>
#endif

#define PORT "80"

using namespace std;

bool rcv_ack_from_server (const int sockfd)
{
    char rcv_pkt[MAXDATA];
    recv(sockfd, rcv_pkt, MAXDATA,MSG_PEEK); //MSG_PEEK option doesn't remove data from buffer
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
        fprintf(stdout, "Error opening file %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //fstat() stats the file pointed to by file descriptor fd and fills in buffer file_stat.
    if (fstat(fd, &file_stat) < 0) {
        fprintf(stdout, "Error fstat --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // get the file_stat size
    offset = 0;
    remain_data = file_stat.st_size;
    cout << "file size = " << remain_data << endl;

    //create buffer for the file
    char* buf[remain_data];
    if (read(fd, buf, remain_data) != remain_data) {
        fprintf(stdout, "Error fstat --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int data_ptr = 0;
    while(remain_data > 0) {
        sent_bytes = send(sockfd, buf+data_ptr, remain_data, 0);
        if(sent_bytes == -1) {
            fprintf(stdout, "Error in sending the file.");
            exit(EXIT_FAILURE);
        }
        data_ptr = sent_bytes;
        remain_data -= sent_bytes;
    }

}

int streq(const char* s1, const char* s2) {
    int counter = 0;
    while(s1[counter] != '\0' && s2[counter] != '\0') {
        if(s1[counter] != s2[counter]) {
            return 0;
        }
        counter++;
    }
    return s2[counter] == '\0';
}

void rcv_file_from_server(const int sockfd, string &filename) {

    char httpmsg[MAXDATA];
    FILE *received_file;
    int remain_data,
    bytes, size_of_payload;
    int header_end = -1;

    const string &path = ("./client/" + filename);
    received_file = fopen(path.c_str(), "w");

    if (received_file == NULL) {
        fprintf(stdout, "Failed to open file %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //get the header and part/full of the wanted file.
    while(header_end == -1) {
        //here we only peek until we are sure that the full header arrived.
        bytes = recv(sockfd, httpmsg, MAXDATA-1, MSG_PEEK);
        //check if full header arrived
        for(int i=0; i<bytes; i++) {
            if(streq(httpmsg+i, "\r\n\r\n") == 1) {
                header_end = i + strlen("\r\n\r\n");
                break;
            }
        }
    }

    //now we will get the data and remove the data from the buffer.
    bytes = recv(sockfd, httpmsg, header_end, MSG_WAITALL);

    //terminate the string to be safe to be used in streq() function.
    httpmsg[bytes] = '\0';

    //find content-length header
    int length_pointer = -1;
    for(int i=0; i<bytes; i++) {
        if(streq(httpmsg+i, "Content-Length:") == 1) {
            length_pointer = i + strlen("Content-Length:");
            break;
        }
    }

    //handle if Content-Length: not found!!
    if (length_pointer == -1) {
        fprintf(stdout, "Failed to receive Content-Length");
        exit(EXIT_FAILURE);
    }

    size_of_payload = atoi(httpmsg+length_pointer);


    //handle if the end of the header not found!!
    if (header_end == -1) {
        fprintf(stdout, "Header end not found!!");
        exit(EXIT_FAILURE);
    }

    //total request size - received bytes
    remain_data = (size_of_payload + header_end) - bytes;

    //write the first piece of the file
    if (fwrite(httpmsg+header_end, sizeof(char), bytes - header_end, received_file) == -1) {
        fprintf(stdout, "Failed to write file %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //data
    cout<< filename<<'\n'<<'\n';
    printf("%s\n", httpmsg);
    printf("%d header end \n", header_end);
    printf("%d pointer length \n", length_pointer);
    printf("%d size of payload \n", size_of_payload);
    printf("%d remaining data \n", remain_data);
    printf("%d received bytes\n", bytes);

    //get the rest of the file
    while(remain_data > 0) {

        bytes = recv(sockfd, httpmsg, min(MAXDATA, remain_data), 0);
        printf("%d received bytes\n", bytes);

        if (bytes == -1) {
            fprintf(stdout, "Socket closed before file receive.");
            exit(EXIT_FAILURE);
        }

        if (fwrite(httpmsg, sizeof(char), bytes, received_file) == -1) {
            fprintf(stdout, "Failed to write file %s\n", strerror(errno));
            exit(EXIT_FAILURE);

        }

        remain_data -= bytes;
    }

    fclose(received_file);
}

void send_header_line (const int sock_fd,const string & method,string &filename)
{

    types_manager tp=types_manager();
    string msg = method + " /" + filename + " HTTP/1.0\r\n";
    msg += tp.generate_file_header(filename)+ "\r\n";

    if (method =="POST") {

        struct stat file_stat;

        int fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1) {
            fprintf(stdout, "Error opening file %s \n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        //fstat() stats the file pointed to by file descriptor fd and fills in buffer file_stat.
        if (fstat(fd, &file_stat) < 0) {
            fprintf(stdout, "Error fstat --> %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        msg = method + " " + filename + " HTTP/1.0\r\n";
        string siz=to_string(file_stat.st_size);
        if (siz=="-1")
        {
            fprintf(stdout, "Error opening file %s \n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else
        msg += "Content-Length: " + siz + "\r\n";

    }

    msg += "\r\n";

    //used when socket in a connected state
    int remain_data = msg.size();
    int data_ptr = 0;
    int sent_bytes = 0;
    while(remain_data > 0) {
        sent_bytes = send(sock_fd, msg.c_str() + data_ptr, remain_data, 0);
        if(sent_bytes == -1) {
            fprintf(stdout, "Error in sending the file.");
            exit(EXIT_FAILURE);
        }
        data_ptr = sent_bytes;
        remain_data -= sent_bytes;
    }
}
/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    string command;
    int sockfd, status;
    struct addrinfo hints;
    struct addrinfo *res;

    // Open the TCPConnection
    if (argc < 2) {
        fprintf(stdout,"Insufficient parameters\n");
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

    // Process the commands
    ifstream commandsfile ("./http_client/commands.txt", ios::in);
    if(commandsfile.is_open()) {

        while(getline(commandsfile, command)) {

            // getaddrinfo() system call
            if ((status = getaddrinfo(argv[1], port_number, &hints, &res)) != 0) {
                fprintf(stdout, "getaddrinfo: %s\n", gai_strerror(status));
                exit(1);
            }
            // socket() system call
            if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
                perror("client: socket");
                exit(1);
            }

            clock_t begin = clock();

            // connect() system call
            if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
                perror("client: connect");
                exit(1);
            }
            vector<string> splitted_cmd = split_cmd(command);
            send_header_line(sockfd, splitted_cmd[0], splitted_cmd[1]);
            if (splitted_cmd[0] == "GET") {
                rcv_file_from_server(sockfd, splitted_cmd[1]);
            } else if (splitted_cmd[0] == "POST") {
                //you must call rcv_ack to make POST work correctly.
                rcv_ack_from_server(sockfd);
                send_file_to_server(sockfd, splitted_cmd[1]);
            }
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            cout << "-------------------------------------------" << endl;
            cout << "\nElapsed time = " << elapsed_secs << " secs\n" << endl;
            cout << "-------------------------------------------" << endl;
            // Close the TCPConnection
            freeaddrinfo(res);
            shutdown(sockfd, 2);
        }
        commandsfile.close();
    }


    return 0;
}
