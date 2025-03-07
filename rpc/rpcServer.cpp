//
// Created by Admin on 2025/2/20.
//

#include "rpcServer.h"




int rpcServer::StartRpcServer()
{
    m_flux.startCorrespond(12345,CallBack);
    return 0;
}

// pBuf -> 接收到的消息
void rpcServer::CallBack(char *pBuf)
{
    cout<<pBuf<<endl;

    //解析pBuf


    return ;
}

int rpcServer::parse() {

    return 0;
}

