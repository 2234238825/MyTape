//
// Created by Admin on 2025/2/14.
//


#include "netFlux.h"


int netFlux::startCorrespond(int port,function<void(char*)> callBack)
{
    cout<<"Start Server"<<endl;

    sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    bind(listen_fd, (sockaddr*)&srv, sizeof(srv));

    listen(listen_fd, SOMAXCONN);

    //poll 数组，用于检测事件。
    std::vector<struct pollfd> fds;
    fds.push_back({listen_fd, POLLIN, 0});

    while(true)
    {
        int ret = poll(fds.data(), fds.size(), -1); // 无限等待
        if(ret == -1)
        {
            cout<<"poll error"<<endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].fd == listen_fd && (fds[i].revents & POLLIN))
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
                if (client_fd == -1)
                {
                    cout << "Failed to accept" << endl;
                    continue;
                }
                fds.push_back({client_fd,POLLIN,0});
            }
            else if(fds[i].revents & POLLIN)
            {
                char buffer[1000];
                memset(buffer,0,sizeof(buffer));
                int read_len = read(fds[i].fd,buffer,sizeof(buffer));
                if(read_len == -1)
                {
                    cout<<"read data error"<<endl;
                    continue;
                }
                else if(read_len == 0)
                {
                    cout<<"Client disconnect"<<endl;

                    close(fds[i].fd);
                    fds.erase(fds.begin()+i);
                    i--;
                }
                else
                {
                    callBack(buffer);
                }
            }
        }
    }
    return 0;
}
