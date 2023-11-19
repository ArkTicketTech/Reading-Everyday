主流的IO模型：

* 堵塞式：Java中Socket对象 `data=socket.read()`
* 非堵塞式：Java中Socket对象
* IO多路复用：Linux中的系统调用select函数、Java的NIO
* 信号驱动：epoll系统
* 异步



I/O模型与Kafka的关系又是什么呢？实际上Kafka客户端底层使用了Java的selector，selector在Linux上的实现机制是epoll，而在Windows平台上的实现机制是select。因此在这一点上将Kafka部署在Linux上是有优势的，因为能够获得更高效的I/O性能。



