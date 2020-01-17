#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>
#include "connection.h"

Connection::Connection(){}

int Connection::start(int nodeId){
    struct sockaddr_in serv_addr;
    char buffer[CONST_MAX_BUFFER] = {0}; 
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    if (connect(this->sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }
    string s_nodeId = to_string(nodeId);
    // cout << "Transmitting..." << endl;
    this->transmit(s_nodeId.c_str());
    // cout << "Receiving " << endl;
    return 1; 
    // cout << buffer << endl;

}

void Connection::transmit(const char *msg){
    // send(this->sock, msg, strlen(msg), 0);
    int n;
    n = write(this->sock,msg, strlen(msg));
    if (n < 0) printf("ERROR writing to socket");
}

int Connection::receive(char *msg){
    int valread = 0;
    valread = read(this->sock, msg, CONST_MAX_BUFFER-1);
//    int n = 0;
//    while(n == 0){
//       bzero(msg,CONST_MAX_BUFFER);
//       n = read(this->sock,msg,CONST_MAX_BUFFER-1);
//    }
//    if (n < 0) printf("ERROR reading from socket");
    return valread;
}
