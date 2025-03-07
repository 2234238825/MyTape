//
// Created by Admin on 2025/2/20.
//

#ifndef UNTITLED13_RPCSERVER_H
#define UNTITLED13_RPCSERVER_H
#include "netFlux.h"
#include "convertJson.h"
using namespace ConvJson;

struct rpcMessage
{
    string methodName;
    
};
class rpcServer
{
    netFlux m_flux;
public:
    int StartRpcServer();
    int parse();
    static void CallBack(char *pBuf);
};


#endif //UNTITLED13_RPCSERVER_H
