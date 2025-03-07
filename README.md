# queueManager接口文档

## 认证接口

+ 接口名
  qm.auth

+ 接口描述
  用于客户端和queueManager之间的安全认证

+ 入参

  | 参数名      | 类型     | 说明                      |
    | -------- | ------ | ----------------------- |
  | username | string | 用户名                     |
  | password | string | 密码（使用queueManager的私钥加密） |

+ 出参

  | 参数名   | 类型     | 说明            |
    | ----- | ------ | ------------- |
  | code  | int    | 状态码           |
  | token | string | 认证通过后返回的token |

## 心跳接口

+ 接口名
  qm.heart_beat

+ 接口描述
  用于客户端定时发送心跳包给queuemanager

+ 入参

  | 参数名   | 类型     | 说明         |
    | ----- | ------ | ---------- |
  | token | string | 认证获取的token |

+ 出参

  | 参数名  | 类型  | 说明  |
    | ---- | --- | --- |
  | code | int | 返回值 |

## 创建队列接口

+ 接口名
  qm.new_queue

+ 接口描述
  用于客户端在dataManager上创建队列

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |

## 添加值到队列接口

+ 接口名
  qm.add

+ 接口描述
  用于客户端将值添加到queueManager上的队列

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |
  | element    | string | 要加入队列的值      |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |

## 批量添加值到队列接口

+ 接口名
  qm.add_all

+ 接口描述
  用于客户端将多个值添加到queueManager上的队列

+ 入参

  | 参数名        | 类型              | 描述           |
    | ---------- | --------------- | ------------ |
  | token      | string          | auth获取到token |
  | queue_name | string          | 队列名称         |
  | elements   | Array\<String\> | 要加入队列的值      |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |

## 从队列获取值接口

+ 接口名
  qm.poll

+ 接口描述
  用于客户端从queueManager上的队列获取值

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |

+ 出参

  | 参数名        | 类型     | 描述    |
    | ---------- | ------ | ----- |
  | code       | int    | 状态码   |
  | element    | string | 获取到的值 |
  | message_id | int64  | 消息id  |

## 从队列获取多个值的接口

+ 接口名
  qm.pull_elements

+ 接口描述
  用于客户端从QueueManager上获取多个值

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |
  | number     | int    | 要获取的值数量      |

+ 出参

  | 参数名          |            | 类型             | 描述         |
    | ------------ | ---------- | -------------- | ---------- |
  | code         |            | int            | 状态码        |
  | element_info |            | array\<String> | 获取元素集合     |
  |              | element    | string         | 获取到的元素     |
  |              | message_id | int            | int64      |
  | number       |            |                | 实际获取到的元素个数 |

## 消息消费确认

+ 接口名
  qm.ack

+ 接口描述
  用于客户端向queueManager通知消息是否消费完成

+ 入参

  | 参数名        | 类型     | 描述                 |
    | ---------- | ------ | ------------------ |
  | token      | string | auth获取的token       |
  | message_id | int64  | 消息id               |
  | ack_type   | int    | 消费状态：0-消费完成，1-出现错误 |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |

## 获取队列大小

+ 接口名
  qm.size

+ 接口描述
  获取queueManager上指定队列的当前大小

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |

+ 出参

  | 参数名  | 类型  | 描述     |
    | ---- | --- | ------ |
  | code | int | 状态码    |
  | size | int | 队列当前大小 |

## 获取队列是否为空

+ 接口名
  qm.is_empty

+ 接口描述
  获取queuemanager上指定队列是否为空

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |

+ 出参

  | 参数名   | 类型      | 描述     |
    | ----- | ------- | ------ |
  | code  | int     | 返回码    |
  | empty | boolean | 队列是否为空 |



## 销毁队列接口

+ 接口名
  qm.destory_queue

+ 接口描述
  销毁queueManager上的队列

+ 入参

  | 参数名        | 类型      | 描述           |
    | ---------- | ------- | ------------ |
  | token      | string  | auth获取的token |
  | queue_name | string  | 队列名称         |
  | force      | boolean | 是否强制删除       |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |

## 获取未返回的ack数量接口

+ 接口名
  qm.get_uncheck_num

+ 接口描述
  获取队列中未返回 ack 的数量

+ 入参

  | 参数名        | 类型     | 描述           |
    | ---------- | ------ | ------------ |
  | token      | string | auth获取的token |
  | queue_name | string | 队列名称         |

+ 出参

  | 参数名     | 类型    | 描述    |
    | ------- | ----- | ----- |
  | code    | int   | 返回码   |
  | ack_num | int64 | ack数量 |

## 添加kv对

+ 接口名
  qm.map_put

+ 接口描述
  在queueManager上添加kv对，map不存在时创建新的map

+ 入参

  | 参数名   | 类型     | 描述           |
    | ----- | ------ | ------------ |
  | token | string | auth获取的token |
  | map   | string | map名         |
  | key   | string | kv对的key      |
  | value | string | kv对的value    |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |

## 删除kv对

+ 接口名
  qm.map_remove

+ 接口描述
  在queueManager上删除kv对

+ 入参

  | 参数名   | 类型     | 描述           |
    | ----- | ------ | ------------ |
  | token | string | auth获取的token |
  | map   | string | map名         |
  | key   | string | 要删除的key      |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 状态码 |

## 查询kv对

+ 接口名
  qm.map_get

+ 接口描述
  在queueManager上查询kv对

+ 入参

  | 参数名   | 类型     | 描述           |
    | ----- | ------ | ------------ |
  | token | string | auth获取的token |
  | map   | string | map名         |
  | key   | string | 要删除的key      |

+ 出参

  | 参数名   | 类型     | 描述                     |
    | ----- | ------ | ---------------------- |
  | code  | int    | 状态码                    |
  | value | string | key对应的值，若key不存在，不填充该参数 |

## 获取整个map

+ 接口名
  qm.map_get_all

+ 接口描述
  获取某个map的所有kv

+ 入参

  | 参数名   | 类型     | 描述           |
    | ----- | ------ | ------------ |
  | token | string | auth获取的token |
  | map   | string | map名         |

  + 出参

  | 参数名  | 类型   | 描述        |
    | ---- | ---- | --------- |
  | code | int  | 状态码       |
  | map  | json | map中所有的kv |



## 销毁map

+ 接口名
  qm.map_destroy

+ 接口描述
  删除某个map

+ 入参

  | 参数名   | 类型     | 描述           |
    | ----- | ------ | ------------ |
  | token | string | auth获取的token |
  | map   | string | map名         |

+ 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 状态码 |

## 获取n个kv对

+ 接口名
  qm.map_get_list

+ 接口描述
  在queueManager上n个kv对

+ 入参

  | 参数名    | 类型     | 描述           |
    | ------ | ------ | ------------ |
  | token  | string | auth获取的token |
  | map    | string | map名         |
  | offset | int    | 从第几个kv对开始获取  |
  | size   | int    | 获取多少个kv对     |

+ 出参

  | 参数名    | 类型   | 描述                             |
    | ------ | ---- | ------------------------------ |
  | code   | int  | 状态码                            |
  | offset | int  | 传入的offset                      |
  | size   | int  | 实际返回的kv对个数                     |
  | list   | json | 获取到的kv对，当map剩余容量不足size时有多少返回多少 |

## 批量插入kv对

+ 接口名
  qm.map_batch_put

+ 接口描述
  在queueManager上put n个kv对

+ 入参

  | 参数名   | 类型     | 描述                                   |
    | ----- | ------ | ------------------------------------ |
  | token | string | auth获取的token                         |
  | map   | string | map名                                 |
  | size  | int    | 传入kv对的数量 |
  | list  | json   | 传入的kv对，每个json的kv对应一个要put的kv对         |

  + 出参

  | 参数名  | 类型  | 描述  |
    | ---- | --- | --- |
  | code | int | 返回码 |
