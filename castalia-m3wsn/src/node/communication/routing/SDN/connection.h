#ifndef SOCKET2_H
#define SOCKET2_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

#define CONST_MAX_BUFFER 8192
#define PORT 8000

using namespace std;

class Connection
{
    private:
        int sock = 0;
    public:
        Connection();
        int start(int nodeId);
        void transmit(const char *msg);
        int receive(char *msg);
};

#endif
