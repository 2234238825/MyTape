//
// Created by Admin on 2025/2/14.
//

#ifndef UNTITLED13_NETFLUX_H
#define UNTITLED13_NETFLUX_H

#include <iostream>
#include <cstring>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <poll.h>
#include <functional>
using namespace std;
typedef void (*m_pfnCallBack)(char *);
class netFlux
{
public:
    int startCorrespond(int port,function<void(char*)> callBack);
};


#endif //UNTITLED13_NETFLUX_H
