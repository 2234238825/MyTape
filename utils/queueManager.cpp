#include "queueManager.h"

string reponse(Json::Value root)
{
    return Json2String(root);
}

queueManager::queueManager()
{
    memset(m_HashMessageId, 0, sizeof(m_HashMessageId));
    m_MessageId = 0;
}

string queueManager::heart_beat()
{
    return R"({"code":0})";
}


string queueManager::new_queue(const string& queue_name)
{
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    if (m_queTask.find(queue_name) != m_queTask.end())
    {
        root["code"] = E_QUEUE_CREATE_QUEUE_FAILED;
    }
    else
    {
        m_queTask[queue_name];
        root["code"] = 0;
    }
    return reponse(root);
}

string queueManager::add(const string& queue_name, string element)
{
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    if (m_queTask.find(queue_name) != m_queTask.end())
    {
        m_queTask[queue_name].emplace_back(element);
        root["code"] = 0;
        return reponse(root);
    }
    root["code"] = E_QUEUE_NOT_FOUND_FAILED;
    return reponse(root);
}

string queueManager::add_all(const string& queue_name, vector<string> elements)
{
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    if (m_queTask.find(queue_name) != m_queTask.end())
    {
        for (int i = 0;i < elements.size();i++)
        {
            m_queTask[queue_name].emplace_back(elements[i]);

        }
        root["code"] = 0;
        return reponse(root);
    }

    root["code"] = E_QUEUE_NOT_FOUND_FAILED;
    return reponse(root);
}

string queueManager::pull(const string& queue_name)
{
    int code = 0;
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    m_MessageId = -1;
    for (int i = 0;i < maxn;i++)
    {
        if (!m_HashMessageId[i])
        {
            m_HashMessageId[i] = true;
            m_MessageId = i;
            break;
        }
    }
    if (m_MessageId == -1)
    {
        code = E_QUEUE_HASH_MESSAGE_FAILED;
        root["code"] = code;
        return reponse(root);
    }

    if (!m_queTask[queue_name].empty())
    {
        m_ackNumber[queue_name]++;
        m_queName[m_MessageId] = queue_name;
        string task = m_queTask[queue_name].front();
        m_queTask[queue_name].pop_front();

        m_messageIdTask[m_MessageId] = task;
        root["code"] = code;
        root["element"] = task;
        root["message_id"] = m_MessageId;
        return reponse(root);
    }
    else
    {
        m_HashMessageId[m_MessageId] = false;
        code = E_QUEUE_QUEUE_ZERO_SIZE_FAILED;
        root["code"] = code;
        return reponse(root);
    }
}

string queueManager::pull_elements(const string& queue_name, int number)
{

    Json::Value root;
    Json::Value result;
    result["code"] = 0;
    int Int = number;
    lock_guard<std::mutex> lock(m_mutex);
    for (int i = 0;i < maxn && number != 0; i++)
    {
        if (!m_HashMessageId[i])
        {
            if (m_queTask[queue_name].empty())
            {
                break;
            }
            m_HashMessageId[i] = true;
            m_MessageId = i;
            number--;
            string task = m_queTask[queue_name].front();
            m_queTask[queue_name].pop_front();

            m_ackNumber[queue_name]++;
            m_queName[m_MessageId] = queue_name;
            m_messageIdTask[m_MessageId] = task;
            root["element"] = task;
            root["message_id"] = m_MessageId;
            result["element_info"].append(root);
        }
    }
    result["number"] = Int - number;

    return reponse(result);
}

string queueManager::ack(int64_t message_id, int ack_type)
{
    lock_guard<std::mutex> lock(m_mutex);
    if (ack_type == 0)
    {
        m_HashMessageId[message_id] = false;

        string queueName = m_queName[message_id];
        m_queName.erase(message_id);

        m_ackNumber[queueName]--;
        m_messageIdTask.erase(message_id);

    }
    else if (ack_type == 1)
    {
        string queueName = m_queName[message_id];
        string task = m_messageIdTask[message_id];

        m_messageIdTask.erase(message_id);
        m_queName.erase(message_id);
        m_HashMessageId[message_id] = false;
        m_ackNumber[queueName]--;

        m_queTask[queueName].emplace_back(task);
    }

    return R"({"code":0})";
}

string queueManager::size(const string& queue_name)
{
    int code = 0;
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    int pSize = m_queTask[queue_name].size();

    root["code"] = code;
    root["size"] = pSize;
    return reponse(root);
}

string queueManager::is_empty(const string& queue_name)
{
    int code = 0;
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    if (m_queTask.find(queue_name) != m_queTask.end())
    {
        root["code"] = code;
        root["empty"] = 0;
    }
    else
    {
        root["code"] = code;
        root["empty"] = 1;
    }
    return reponse(root);
}

string queueManager::destroy_queue(const string& queue_name, bool force)
{
    int code = 0;
    Json::Value root;
    lock_guard<std::mutex> lock(m_mutex);
    if (force == 1)
    {
        m_queTask.erase(queue_name);
    }
    else
    {
        if (!m_queTask[queue_name].empty())
        {
            return R"({"code":0})";
        }
        else m_queTask.erase(queue_name);
    }
    return R"({"code":0})";
}

string queueManager::get_uncheck_num(const string& queue_name) {
    int code = 0;
    Json::Value root;
    root["code"] = code;
    lock_guard<std::mutex> lock(m_mutex);
    int ack_num = m_ackNumber[queue_name];
    root["ack_num"] = ack_num;
    return reponse(root);
}

string queueManager::map_put(const string& pmapName, const string& pkey, const string& pvalue) {
    lock_guard<std::mutex> lock(p_mutex);

    m_MapMap[pmapName][pkey] = pvalue;
    return R"({"code":0})";
}

string queueManager::map_remove(const string& pmapName, const string& pkey) {
    lock_guard<std::mutex> lock(p_mutex);
    m_MapMap[pmapName].erase(pkey);
    return R"({"code":0})";
}

string queueManager::map_get(const string& pmapName, const string& pkey) {
    int code = 0;
    Json::Value root;
    root["code"] = code;
    lock_guard<std::mutex> lock(p_mutex);
    if(m_MapMap[pmapName].find(pkey) != m_MapMap[pmapName].end())
    {
        string pMapMapValue = m_MapMap[pmapName][pkey];
        root["value"] = pMapMapValue;
    }

    return reponse(root);
}

string queueManager::map_get_all(const string& pmapName) {
    Json::Value root;
    root["code"] = 0;

    lock_guard<std::mutex> lock(p_mutex);
    for (auto iter = m_MapMap[pmapName].begin(); iter != m_MapMap[pmapName].end();iter++)
    {
        root["map"][iter->first] = iter->second;
    }

    return reponse(root);
}

string queueManager::map_destroy(const string& pmapName) {
    Json::Value root;
    root["code"] = 0;
    lock_guard<std::mutex> lock(p_mutex);
    m_MapMap.erase(pmapName);
    return reponse(root);
}

string queueManager::map_batch_put(const string &pmapName, int pSize, map<string, string> K_Value) {
    Json::Value root;
    root["code"] = 0;
    lock_guard<std::mutex> lock(p_mutex);
    for(auto iter = K_Value.begin(); iter != K_Value.end();iter ++)
    {
        m_MapMap[pmapName][iter->first] = iter->second;
    }
    return reponse(root);
}

string queueManager::map_get_list(const string &pmapName, int offset, int size) {
    Json::Value root;
    root["code"] = 0;
    root["offset"] = offset;
    map<int ,int > p;
    lock_guard<std::mutex> lock(p_mutex);
    if(m_MapMap.find(pmapName) == m_MapMap.end())
    {
        root["list"] = "";
        root["size"] = 0;
        return reponse(root);
    }
    auto iter = m_MapMap[pmapName].begin();
    advance(iter,offset);
    if(iter == m_MapMap[pmapName].end())
    {
        root["list"] = "";
        root["size"] = 0;
        return reponse(root);
    }

    for(; iter != m_MapMap[pmapName].end() && size != 0; iter++,size--)
    {
        root["list"][iter->first] = iter->second;
    }


    return reponse(root);
}

