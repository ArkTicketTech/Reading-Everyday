从0入门微服务

# 初探微服务

服务调用依赖于下面几个基本组件：

1. 服务描述：Restful API
2. 注册中心：服务提供者向注册中心注册自己的服务，服务消费者根据注册中心的地址发起调用
3. 服务框架：

1. 服务通信采用什么协议？http还是？
2. 数据传输采用什么方式？同步还是异步、单连接上传输还是多路复用
3. 数据压缩采用什么格式？Json序列化，Java对象序列化

1. 服务监控：指标收集、数据处理、数据展示
2. 服务追踪：调用时传traceId，追踪整个调用链路
3. 服务治理：保证在各种意外情况下，服务调用仍然能够正常运行，常见的故障有单机故障、单IDC（互联网数据中心）故障、依赖服务不可用

# 如何注册和发现服务？

1. 服务提供者（RPC server）：向Registry注册自身服务，并向Registry定期发送心跳汇报存活状态
2. 服务注册中心（Registry）：监控并更新服务提供者的变化，有变更就会立即通知服务消费者刷新缓存
3. 服务消费者（RPC client）：向Registry订阅服务，拉取服务地址然后缓存在本地内存中。


注册中心的存在，可以解耦服务提供者和消费者，服务提供者可以任意伸缩节点。

注册中心一般采用分布式集群部署，来保证可用性，有的注册中心还采用多IDC部署。

# 如何实现RPC远程服务调用？

remote procedure call

1. 客户端与服务端首先要建立连接：

1. HTTP通信：基于应用层HTTP协议，HTTP协议基于传输层TCP协议
2. Socket通信：基于TCP/IP协议，socket本身不是协议，而是一个调用接口，通过socket，才能使用Tcp/Ip协议。

1. 然后按照某个约定的协议进行网络通信：
2. 服务端收到请求后，进行处理后的结果再返回给消费者
3. 为了减少传输的数据大小，还需要对数据进行压缩，也就是对数据进行序列化

## 客户端和服务端如何建立网络连接？

http和socket通信

## socket通信

建立一次socket连接至少需要一对socket，

1. 其中一个运行于客户端，称为ClientSocket
2. 另一个运行于服务端，称为ServerSocket

socket通信分为四个步骤：

1. 服务器监听：ServerSocket通过调用bind()函数绑定某个具体端口，然后调用listen()函数实时监听网络状态，等待客户端的连接请求
2. 客户端请求：ClientSocket调用connect()函数向ServerSocket绑定的地址和端口发起连接请求
3. 服务器连接确认：当 ServerSocket 监听到或者接收到 ClientSocket 的连接请求时，调用 accept() 函数响应 ClientSocket 的请求，同客户端建立连接。
4. 数据传输：当 ClientSocket 和 ServerSocket 建立连接后，ClientSocket 调用 send() 函数，ServerSocket 调用 receive() 函数，ServerSocket 处理完请求后，调用 send() 函数，ClientSocket 调用 receive() 函数，就可以得到得到返回结果。


网络不是可靠的，遇到异常时通常的处理手段有两种

1. 链路存活检测：客户端需要定时发送心跳检测消息给服务端，服务端连续n次或者规定时间内没有回复消息，则认为链路失效。客户端则需要重新与服务端建立请求
2. 断连重试：等待一定的时间间隔后再发起重连。

## 服务端如何处理请求

1. 同步阻塞BIO：服务端创建线程处理请求

1. 适用于连接比较小的业务场景

1. 同步非阻塞NIO：服务端通过I/O多路复用技术处理请求

1. 适用于连接数比较多并且请求消耗比较轻的业务场景，比如聊天服务器

1. 异步非阻塞AIO：

1. 适用于连接数比较多并且请求消耗比较轻的业务场景

## 数据传输采用什么协议？

1. HTTP
2. Dubbo

## 序列化和反序列化

1. JSON可读性好，性能差一点
2. XML可读性不好
3. 二进制类PB/Thrift



# 如何监控微服务调用？

## 监控对象

监控对象分为四个层次：

1. 用户端监控：功能监控
2. 接口监控
3. 资源监控：mysql/redis/es
4. 基础监控：k8s/cpu/network/磁盘



## 监控指标

1. 请求量
2. 响应时间：平均耗时；百分比耗时，P99=500ms，99%的请求响应时间都在500毫秒以内



## 监控维度

1. 全局维度
2. 分机房维度
3. 单机维度
4. 时间维度：环比等
5. 核心维度：核心业务

## 监控原理

数据采集

1. 服务主动上报：调接口、发消息
2. 代理收集：skywalking



数据传输：

1. UDP传输
2. kafka传输



数据处理：聚合处理

聚合维度：

1. 接口维度
2. 机器维度



聚合结果持久化：

1. 索引数据库：es/clickhouse
2. 时序数据库：



数据展示：grafana等

# 微服务治理等手段



1. 注册中心宕机
2. 服务提供者有节点宕机
3. 服务消费者和注册中心的网络不通
4. 服务提供者和注册中心网络不通
5. 服务消费者和服务提供者网络不通
6. 服务提供者节点性能变慢

## 节点管理

1. 注册中心主动摘除机制
2. 服务消费者摘除机制：如果注册中心与服务提供者网络不通，但实际上服务提供者是可用的，所以由消费者探测存活可能更合理

## 负载均衡

1. 随机算法
2. 轮询算法
3. 最少活跃调用算法
4. 一致性hash算法

## 服务路由

对于服务消费者而言，在内存中的可用服务节点列表中选择哪个节点不仅由负载均衡算法决定，还由路由规则确定。

为什么需要路由规则？

1. 业务存在灰度发布的需求
2. 多机房就近访问的需求



路由规则放在注册中心。

## 服务容错

1. failover：故障切换，选择新节点重新发起调用
2. failback：故障通知，不再重试，根据失败的详细信息，来决定后续执行策略。
3. failcache：故障缓存，隔一段时间再进行重试
4. failfast：快速失败
