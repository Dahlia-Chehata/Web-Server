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
#include <zconf.h>
#include <unistd.h>


#define PORT "80"

using namespace std;

bool rcv_ack_from_server (const int sockfd)
{
    char rcv_pkt[MAXDATA];
    recv(sockfd, rcv_pkt, MAXDATA,MSG_PEEK); //MSG_PEEK option doesn't remove data from buffer
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
        fprintf(stderr, "Error opening file %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //fstat() stats the file pointed to by file descriptor fd and fills in buffer file_stat.
    if (fstat(fd, &file_stat) < 0) {
        fprintf(stderr, "Error fstat --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // get the file_stat size
    sprintf(file_size, "%jd", file_stat.st_size);
    offset = 0;
    remain_data = file_stat.st_size;
    cout << "file size = " << file_size << endl;

    while (((remain_data > 0) && (sent_bytes = sendfile(sockfd, fd, &offset, MAXDATA)) > 0)  ) {
      remain_data -= sent_bytes;
      cout << "remain data = " << remain_data << endl;
    }
//    if (remain_data)
//    {
//        sendfile(sockfd, fd, &offset, remain_data);
//        cout << "remain data = 0" << endl;
//    }
}
//void gotoxy(int x,int y)
//{
//    printf("%c[%d;%df",0x1B,y,x);
//}
//void rcv_file_from_server(const int sockfd,string &filename)
//{
//    /* Create file where data will be stored */
//    int bytesReceived = 0;
//    char recvBuff[256];
//    memset(recvBuff, '0', sizeof(recvBuff));
//    FILE *fp;
//    read(sockfd, strdup(filename.c_str()), 1);
//    //strcat(fname,"AK");
//    printf("File Name: %s\n",filename.c_str());
//    printf("Receiving file...\n");
//    fp = fopen(filename.c_str(), "ab");
//    if(NULL == fp)
//    {
//        printf("Error opening file\n");
//        exit(1);
//    }
//    long double sz=1;
//    /* Receive data in chunks of 256 bytes */
//    while((bytesReceived = read(sockfd, recvBuff, 1024)) > 0)
//    {
//        sz++;
//        gotoxy(0,4);
//        printf("Received: %llf Mb",(sz/1024));
//      //  fflush(stdout);
//        // recvBuff[n] = 0;
//        fwrite(recvBuff, 1,bytesReceived,fp);
//        fflush(stdout);
//        // printf("%s \n", recvBuff);
//    }
//    fclose(fp);
//
//    if(bytesReceived < 0)
//    {
//        printf("\n Read Error \n");
//    }
//    printf("\nFile OK....Completed\n");
//}
void rcv_file_from_server(const int sockfd, string &filename) {
    char httpmsg[MAXDATA];
    FILE *received_file;
    int remain_data,
     bytes,total_rcv;

    const string &path = ("../web/" + filename);
    received_file = fopen(path.c_str(), "r");

    if (received_file == NULL) {
        fprintf(stderr, "Failed to open file %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    remain_data = stoi(to_string(file_size(path.c_str())));
    printf("file :%s of size : %d \n", filename.c_str(), remain_data);

    // receive file data
    while (1) {
        bytes = recv(sockfd, httpmsg+total_rcv, MAXDATA-total_rcv, 0);
        if (bytes == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        if (bytes == 0) {
            cout << "Problem in receiving the rest of the data" << endl;
            break;
        }
        if (fwrite(httpmsg, sizeof(char), bytes, received_file) == -1) {
            fprintf(stderr, "Failed to write file %s\n", strerror(errno));
            exit(EXIT_FAILURE);

        }
        if (bytes>0)
        total_rcv += bytes;
        fprintf(stdout, " %d bytes received and %d bytes remaining\n", bytes, remain_data=remain_data-bytes);
    }
    fclose(received_file);
}

void send_header_line (const int fd,const string & method,string &filename)
{
    struct stat file_stat;
    if (fd == -1) {
        fprintf(stderr, "Error opening file %s \n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //fstat() stats the file pointed to by file descriptor fd and fills in buffer file_stat.
    if (fstat(fd, &file_stat) < 0) {
        fprintf(stderr, "Error fstat --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    types_manager tp=types_manager();
    string msg = method + " " + filename + " HTTP/1.0\r\n";
    msg += tp.generate_file_header(filename)+ "\r\n";
    if (method =="POST") {
        string siz=to_string(file_size(filename.c_str()));
        if (siz=="-1")
        {
            fprintf(stderr, "Error opening file %s \n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else
        msg += "Content-Length: " + siz + "\r\n";
    }
    msg += "\r\n";
    //used when socket in a connected state
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
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

    // Process the commands
    ifstream commandsfile ("commands.txt", ios::in);
    if(commandsfile.is_open()) {

        while(getline(commandsfile, command)) {

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

            // connect() system call
            if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
                perror("client: connect");
                exit(1);
            }
            vector<string> splitted_cmd = split_cmd(command);
            send_header_line(sockfd, splitted_cmd[0], splitted_cmd[1]);
          //  bool positive_ack = rcv_ack_from_server(sockfd);
            if (splitted_cmd[0] == "GET") {
                 rcv_file_from_server(sockfd, splitted_cmd[1]);
            } else if (splitted_cmd[0] == "POST") {
//           if (positive_ack)
                  send_file_to_server(sockfd, splitted_cmd[1]);
//               else
//                printf("ERROR in POST");
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
