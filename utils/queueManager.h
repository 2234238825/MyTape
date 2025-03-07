//
// Created by Admin on 2025/2/13.
//

#ifndef UNTITLED13_QUEUEMANAGER_H
#define UNTITLED13_QUEUEMANAGER_H

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
#include <map>
#include <mutex>
#include <deque>
#include "convertJson.h"
using namespace ConvJson;
using namespace std;
const int maxn = 1e5;
struct transferData
{
    int cmd;
    char s[0];
};

enum
{
    E_QUEUE_ERROR_CODE_START = 1111600000,        /* queueMgr错误码的起始值 */
    E_QUEUE_FAILED,                               /* SSW failed | SSW通用失败 */
    E_QUEUE_CREATE_QUEUE_FAILED,                  /* 创建队列失败 */
    E_QUEUE_NOT_FOUND_FAILED,                      /* 找不到队列 */
    E_QUEUE_HASH_MESSAGE_FAILED,                      /* message_id消耗完 */
    E_QUEUE_QUEUE_ZERO_SIZE_FAILED                    /* 队列无数据 */
};

class queueManager
{
    int64_t m_MessageId;
    int time_out;
    mutex m_mutex;
    mutex p_mutex;
    map<string, int>m_ackNumber;     //队列ack数量
    map<string,deque<string> > m_queTask;
    map<int64_t, deque<string> > m_hashQueue;
    map<int64_t, string >m_queName;    //messageId 对应的队列名
    map<string, map<string, string> >m_MapMap;

    map<int64_t, string >m_messageIdTask;    //messageId 对应的taskString
    bool m_HashMessageId[maxn];


public:
    queueManager();
    int Init(bool CS);
    string heart_beat();
    string new_queue(const string& queue_name);
    string add(const string& queue_name, string element);
    string add_all(const string& queue_name, vector<string> elements);
    string pull(const string& queue_name);
    string pull_elements(const string& queue_name, int number);
    string ack(int64_t message_id, int ack_type);
    string size(const string& queue_name);
    string is_empty(const string& queue_name);
    string destroy_queue(const string& queue_name, bool force);
    string get_uncheck_num(const string& queue_name);
    string map_put(const string& pmapName,const string& pkey, const string& pvalue);
    string map_remove(const string& pmapName, const string& pkey);
    string map_get(const string& pmapName, const string& pkey);
    string map_get_all(const string& pmapName);
    string map_destroy(const string& pmapName);
    string map_batch_put(const string& pmapName, int pSize, map<string, string> K_Value);
    string map_get_list(const string& pmapName, int offset, int size);

private:
    int startServer();
    int startClient();
};


#endif //UNTITLED13_QUEUEMANAGER_H
