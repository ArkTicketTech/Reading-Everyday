#### Content delivery network (CDN) 

CDN 是一个由地理位置分散的服务器组成的网络，用于传输静态内容。CDN 服务器会缓存图片、视频、CSS、JavaScript 文件等静态内容。

动态内容缓存是一个相对较新的概念，超出了本书的范围。

以下是 CDN 的高级工作原理：当用户访问网站时，离用户最近的 CDN 服务器将传输静态内容。直观地说，用户离 CDN 服务器越远，网站加载速度越慢。例如，如果 CDN 服务器位于旧金山，那么洛杉矶的用户获取内容的速度就会比洛杉矶的用户快。洛杉矶的用户获取内容的速度将快于欧洲的用户。

![](https://inasa.dev/image/systemdesign/01/1.png)

![](https://inasa.dev/image/systemdesign/01/2.png)

##### Considerations of using a CDN

-   花费
    -   CDN 由第三方提供商运行，您需要为进出 CDN 的数据传输付费。缓存不经常使用的资产不会带来明显的好处，因此你应该考虑将它们移出 CDN。
-   合适的过期
    -   对于时间敏感的内容，设置缓存过期时间非常重要。缓存过期时间既不能太长，也不能太短。如果 太长，内容可能不再新鲜。如果时间太短，可能会导致重复 将内容从源服务器重新加载到 CDN。
-   CDN回退
    -   您应该考虑您的网站/应用程序如何应对 CDN 故障。如果 CDN 出现暂时中断，客户端应能检测到问题，并从源端请求资源。
-   无效文件
    -   使用API删除
    -   增加文件版本

![](https://inasa.dev/image/systemdesign/01/3.png)

-   静态资产（JS、CSS、图片等）不再由网络服务器提供。它们 从 CDN 获取，以提高性能。



#### Stateless web tier

考虑横向扩展网络层。为此，需要将状态 (例如用户会话数据）移出网络层。一个好的做法是将会话数据存储在 持久存储中，如关系数据库或 NoSQL。集群中的每个网络服务器都可以访问数据库中的状态数据。这就是所谓的无状态网络层。

#### Stateful architecture

有状态服务器和无状态服务器有一些主要区别。有状态服务器从一个请求到下一个请求都会记住客户端数据（状态）。无状态服务器不保存任何状态信息。

![](https://inasa.dev/image/systemdesign/01/4.png)

来自同一个客户端的每个请求都必须路由到同一个服务器。这可以通过大多数负载平衡器中的粘性会话来实现；但这会增加开销。添加删除服务器也困难得多。



#### Stateless architecture

![](https://inasa.dev/image/systemdesign/01/5.png)

在这种无状态架构中，用户的 HTTP 请求可以发送到任何网络服务器，而网络服务器可以从共享数据存储区获取状态数据。状态数据存储在共享数据存储区中，不与网络服务器相连。网络服务器之外。无状态系统更简单、更健壮、可扩展。

![](https://inasa.dev/image/systemdesign/01/6.png)



#### Data centers

![](https://inasa.dev/image/systemdesign/01/7.png)

-   `geoDNS` 是一种 `DNS` 服务，可根据用户所在位置将域名解析为 IP 地址。根据用户的位置将域名解析为 IP 地址。
-   多数据中心的问题
    -   `Traffic redirection`
    -   `Data synchronization`
    -   `Test and deployment`



#### Message queue

解耦使消息队列成为构建可扩展的可靠应用程序的首选架构。使用消息队列，生产者可以在消费者无法处理消息时将消息发布到队列中。即使生产者不可用，消费者也能从队列中读取消息。

#### Logging, metrics, automation

-   Logging
    -   监控错误日志非常重要，因为它有助于识别系统中的错误和问题。可以在每个服务器级别监控错误日志，也可以使用工具将错误日志汇总到一个集中服务中，以便于搜索和查看。
-   Metrics
    -   收集不同类型的指标有助于获得业务洞察力并了解系统的健康状况。以下一些指标非常有用：
        -   Host level metrics: CPU, Memory, disk I/O, etc.
        -   Aggregated level metrics: for example, the performance of the entire database tier, cache
            tier, etc.
        -   Key business metrics: daily active users, retention, revenue, etc.
-   Automation
    -   DEVOPS

#### Database scaling

##### Vertical scaling

略

##### Horizontal scaling

![](https://inasa.dev/image/systemdesign/01/8.png)

![](https://inasa.dev/image/systemdesign/01/9.png)

分片是一种扩展数据库的绝佳技术，但它远非完美的解决方案。它给系统带来了复杂性和新的挑战：

-   `Resharding data`
    -   一致性哈希
-   `Celebrity problem`
    -   aka, `hotspot key problem`
    -   对特定分区的过多访问可能会导致服务器超载。试想一下，凯蒂-佩里、贾斯汀-比伯和 Lady Gaga 的数据都在同一个分片上。对于社交应用来说，读取操作将使该分区不堪重负。为了解决这个问题，可能需要为每个名人分配一个分区。每个分区甚至可能需要进一步分区。
-   `Join and de-normalization`
    -   一旦数据库在多个服务器上分片，就很难在数据库分片之间执行连接操作。很难跨数据库分片执行连接操作。常见的解决方法是对数据库进行去规范化处理，以便在单个表中执行查询。



#### Millions of users and beyond

扩展系统是一个迭代的过程。



Pp. 17-33