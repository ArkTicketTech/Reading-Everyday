## Chapter 4 数据编码与演化

编码的

* backward compatibility
* forward compatibility

in-memory representation: objects, structs, lists, arrays, hash tables, trees

a byte sequence: some kind of self-contained sequence of bytes: JSON document

in-memory representation——>a byte sequence

* encoding
* serialization
* marshaling

a byte sequence——>in-memory representation

* decoding
* deserialization
* unmarshaling

服务器：可以是另一项服务的客户端

Web应用Server：作为database的client

### RPC

服务发现：client查询在哪个IP地址和端口号上获得特定的服务。

RPC框架：侧重于同一组织内多项服务之间（通常在同一数据中心内）的请求。

RPC——>databases: asynchronous message-passing system 异步消息传递系统

* message broker
* message queue
* message-oriented middleware

FOSS开源: RabbitMQ, ActiveMQ, Apache Kafka

pros:

* don't enforce any particular data model, can use any encoding format
* change publishers and consumers independently and deploy them in any order