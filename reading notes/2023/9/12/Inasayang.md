### CHAPTER 11: DESIGN A NEWS FEED SYSTEM

本章要求你设计一个信息流系统。什么是信息流？根据 Facebook 帮助页面的介绍，"信息流是你主页中间不断更新的故事列表。信息流包括状态更新、照片、视频、链接、应用程序活动以及你在 Facebook 上关注的人、页面和群组的赞"。这是一个流行的面试问题。常见的类似问题还有：设计 Facebook 信息流、Instagram 消息流、Twitter 时间线等。

![](https://inasa.dev/image/systemdesign/11/1.png)

#### Step 1 - Understand the problem and establish design scope

-   Candidate: 移动端？web端？都有？
-   Interviewer:  都有
-   Candidate: 重要的功能是哪些？
-   Interviewer: 用户可以发布帖子，可以在消息流页面看到朋友的帖子
-   Candidate: 消息流是按时间倒序排序，还是按主题得分等特定顺序排列？例如，来自好友的帖子得分更高
-   Interviewer:  为了简单起见，我们假设消息流按时间倒序排序
-   Candidate: 一个用户可以有多少个朋友
-   Interviewer:  5000
-   Candidate:  流量多大
-   Interviewer:  1 千万 DAU
-   Candidate:  消息包含图片，视频，还是指示文字？
-   Interviewer:  它可以包含媒体文件，包括图像和视频



#### Step 2 - Propose high-level design and get buy-in

设计分为两个流程：消息流发布和消息流构建

-   Feed publishing
    -   当用户发表文章时，相应的数据会被写入缓存和数据库。帖子会被填充到其好友的消息流中
-   Newsfeed building
    -   为简单起见，假设消息流是按时间倒序聚合朋友的帖子

##### Newsfeed APIs

消息流API 是客户端与服务器通信的主要方式。这些 API 基于 HTTP，允许客户端执行各种操作，包括发布状态、检索新闻提要、添加好友等。将讨论两个最重要的 API：消息流发布 API 和消息流检索 API

-   Feed publishing API
    -   POST /v1/me/feed
    -   Params:
        -   content: content is the text of the post
        -   auth_token: it is used to authenticate API requests
-   Newsfeed retrieval API
    -   GET /v1/me/feed
    -   Params:
        -   auth_token: it is used to authenticate API requests



##### Feed publishing

![](https://inasa.dev/image/systemdesign/11/2.png)

-   User: a user can view news feeds on a browser or mobile app. A user makes a post with content “Hello” through API: /v1/me/feed?content=Hello&auth_token={auth_token}
-   Load balancer
-   Web servers: web servers redirect traffic to different internal services
-   Post service: persist post in the database and cache
-   Fanout service: push new content to friends’ news feed. Newsfeed data is stored in the cache for fast retrieval.
-   Notification service: inform friends that new content is available and send out push notifications



##### Newsfeed building

![](https://inasa.dev/image/systemdesign/11/3.png)

-   Newsfeed service: news feed service fetches news feed from the cache
-   Newsfeed cache: store news feed IDs needed to render the news feed



#### Step 3 - Design deep dive

##### Feed publishing deep dive

![](https://inasa.dev/image/systemdesign/11/4.png)

-   Web servers
    -   除了与客户端通信外，网络服务器还执行身份验证和速率限制
    -   只有使用有效 auth_token 登录的用户才能发帖。系统会限制用户在一定时间内发表帖子的数量，这对防止垃圾和滥用内容至关重要
-   Fanout service
    -   扇出是向所有好友发送帖子的过程
    -   扇出模式有两种：写时扇出（又称推送模式）和读时扇出（又称拉取模式）两种模式各有利弊
        -   Fanout on write: 采用这种方法，消息流在写入时就已预先计算好。新文章发布后，会立即发送到好友的缓存中
            -   Pros
                -   消息流实时生成，可立即推送给好友
                -   由于消息流在写入时已预先计算，因此获取消息流的速度很快
            -   Cons
                -   如果用户有很多好友，那么获取好友列表并为所有好友生成消息流的速度就会很慢且耗时, 这就是所谓的热键问题
                -   对于不活跃用户或很少登录的用户，预计算消息流会浪费计算资源
        -   Fanout on read: 消息流在阅读时间内生成。这是一种按需模式。当用户加载主页时，就会提取最近的文章
            -   Pros
                -   对于不活动用户或很少登录的用户，读取时扇出（fanout）的效果更好，因为不会在他们身上浪费计算资源
                -   数据不会推送给好友，因此不存在热键问题
            -   Cons
                -   由于没有预先计算新闻提要，因此获取消息流的速度很慢

采用了一种混合方法，以获得两种方法的优势，并避免其中的缺陷

由于快速获取消息流至关重要，对大多数用户采用推送模式

对于名人或拥有众多好友/关注者的用户，让关注者按需获取新闻内容，以避免系统超载

一致散列是缓解热键问题的有效技术，因为它有助于更均匀地分配请求/数据

![](https://inasa.dev/image/systemdesign/11/5.png)

扇出服务的工作原理如下：

-   从图数据库中获取好友 ID。图数据库适用于管理好友关系和好友推荐

-   从用户缓存中获取好友信息。然后，系统会根据用户设置过滤掉好友。例如，如果你将某人静音，她的帖子就不会显示在你的新闻推送中，即使你们仍然是好友。帖子不显示的另一个原因是，用户可以选择性地与特定好友分享信息，或对其他人隐藏信息

-   将好友列表和新帖 ID 发送至消息队列

-   Fanout Worker 从消息队列中获取数据，并将消息流数据存储在消息流缓存中。你可以把消息流缓存看成一个 <post_id, user_id> 映射表。每当有新的帖子发布，它就会被添加到消息流表中。如果在缓存中存储整个用户和帖子对象，内存消耗会变得非常大。因此，只存储 ID。为了保持较小的内存容量，设置一个可配置的限制。用户在消息流中滚动浏览数千条帖子的可能性很小。大多数用户只对最新内容感兴趣，因此缓存丢失率很低

    ![](https://inasa.dev/image/systemdesign/11/6.png)



##### Newsfeed retrieval deep dive

![](https://inasa.dev/image/systemdesign/11/7.png)

-   用户发送请求检索其消息流
-   负载平衡器将请求重新分配给网络服务器
-   网络服务器调用消息流服务获取消息流
-   消息流服务从消息流缓存中获取帖子 ID 列表
-   用户的消息流不仅仅是一个消息流 ID 列表。它包含用户名、个人资料图片、帖子内容、帖子图片等。因此，消息流服务会从缓存（用户缓存和帖子缓存）中获取完整的用户和帖子对象，以构建fully hydrated的消息流
-   fully hydrated的消息流会以 JSON 格式返回给客户端进行呈现

##### Cache architecture

![](https://inasa.dev/image/systemdesign/11/8.png)

-   News Feed: It stores IDs of news feeds
-   Content: It stores every post data. Popular content is stored in hot cache
-   Social Graph: It stores user relationship data
-   Action: It stores info about whether a user liked a post, replied a post, or took other actions on a post
-    Counters: It stores counters for like, reply, follower, following, etc



#### Step 4 - Wrap up

与任何系统设计面试题一样，系统设计没有完美无缺的方法。每家公司都有其独特的限制条件，你必须设计出适合这些限制条件的系统。了解设计和技术选择的利弊得失非常重要。如果还有几分钟时间，你们可以讨论可扩展性问题。为避免重复讨论，下面只列出了高层次的讨论要点。

Scaling the database

-   Vertical scaling vs Horizontal scaling
-   SQL vs NoSQL
-   Master-slave replication
-   Read replicas
-   Consistency models
-   Database sharding

Other talking points

-   Keep web tier stateless
-   Cache data as much as you can
-   Support multiple data centers
-   Lose couple components with message queues
-   Monitor key metrics. For instance, QPS during peak hours and latency while users refreshing their news feed are interesting to monitor.



Pp. 166-177