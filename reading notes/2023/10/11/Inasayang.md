### CHAPTER 13: DESIGN A SEARCH AUTOCOMPLETE SYSTEM

#### Step 1 - Understand the problem and establish design scope

-   Candidate：是否只在搜索查询开始时才支持匹配，还是在中间也支持？
-   Interviewer：仅在搜索查询开始时使用。
-   Candidate：系统应返回多少条自动完成建议？
-   Interviewer：5
-   Candidate：系统如何知道要返回哪 5 条建议？
-   Interviewer：这取决于受欢迎程度，由历史查询频率决定。
-   Candidate：系统支持拼写检查吗？
-   Interviewer：不支持拼写检查或自动更正。
-   Candidate：搜索查询是否使用英语？
-   Interviewer：如果最后时间允许，我们可以讨论多语言支持。
-   Candidate：是否允许大写和特殊字符？
-   Interviewer：不，我们假设所有搜索查询都使用小写字母字符。
-   Candidate：有多少用户使用该产品？
-   Interviewer：10 million DAU.



Requirements

-   Fast response time
    -   当用户输入搜索查询时，自动完成建议的显示速度必须足够快。一篇关于 Facebook 自动完成系统的文章显示，该系统需要在 100 毫秒内返回结果。否则会造成卡顿
-   Relevant
    -   自动完成建议应与搜索词相关
-   Sorted
    -   系统返回的结果必须按受欢迎程度或其他排序模式排序
-   Scalable
    -   系统可处理高流量
-   Highly available]
    -   当部分系统离线、速度变慢或出现意外网络错误时，系统应保持可用性和可访问性



Back of the envelope estimation

-   Assume 10 million daily active users (DAU)
-   An average person performs 10 searches per day
-   20 bytes of data per query string
    -   Assume we use ASCII character encoding. 1 character = 1 byte
    -   Assume a query contains 4 words, and each word contains 5 characters on average
    -   That is 4 x 5 = 20 bytes per query
-   在搜索框中每输入一个字符，客户端就会向后台发送一个请求，以获取自动完成建议。每个搜索查询平均会发送 20 个请求
-   ~24,000 query per second (QPS) = 10,000,000 users * 10 queries / day * 20 characters / 24 hours / 3600 seconds
-   Peak QPS = QPS * 2 = ~48,000
-   Assume 20% of the daily queries are new. 10 million * 10 queries / day * 20 byte per query * 20% = 0.4 GB. This means 0.4GB of new data is added to storage daily





#### Step 2 - Propose high-level design and get buy-in

-   Data gathering service
    -   它收集用户输入的查询并实时汇总。实时处理对于大型数据集来说并不现实，但这是一个很好的起点
-   Query service
    -   给定搜索查询或前缀，返回 5 个最常搜索的术语



##### Data gathering service

假设有一个频率表，该表存储查询字符串及其频率。一开始，频率表是空的。随后，用户依次输入查询 "twitch"、"twitter"、"twitter "和 "twillo"。

![](https://inasa.dev/image/systemdesign/13/1.png)

##### Query service

假设有一张频率表 

![](https://inasa.dev/image/systemdesign/13/2.png)

-   Query: 会存储查询字符串
-   Frequency: 表示查询的搜索次数

当用户在搜索框中输入 "tw "时，会显示以下前 5 个搜索结果

![](https://inasa.dev/image/systemdesign/13/3.png)

要获得搜索频率最高的 5 个查询，请执行以下 SQL 查询：

![](https://inasa.dev/image/systemdesign/13/4.png)

当数据集较小时，这是一种可以接受的解决方案。当数据集较大时，访问数据库就会成为瓶颈。



#### Step 3 - Design deep dive

-   Trie data structure
-   Data gathering service
-   Query service
-   Scale the storage
-   Trie operations

##### Trie data structure

在高层设计中，关系数据库用于存储。然而，从关系数据库中获取前 5 个搜索查询的效率很低。为了解决这个问题，使用 trie（前缀树）数据结构。由于 trie 数据结构对系统至关重要，将投入大量时间设计定制的 trie。

Trie（发音为 "try"）是一种树状数据结构，可以紧凑地存储字符串。其名称来源于检索一词，表明它是为字符串检索操作而设计的。Trie 的主要思想包括以下几点：

-   一种树状数据结构
-   根代表空字符串
-   每个节点存储一个字符，并有 26 个子节点，每个可能的字符有一个子节点。为了节省空间，我们不绘制空链接
-   每个树节点代表一个单词或前缀字符串

![](https://inasa.dev/image/systemdesign/13/5.png)

基本 trie 数据结构将字符存储在节点中。为了支持按频率排序、 频率信息需要包含在节点中。

![](https://inasa.dev/image/systemdesign/13/6.png)

为节点添加频率信息后，更新三元组数据结构

![](https://inasa.dev/image/systemdesign/13/7.png)

-   p: length of a prefix
-   n: total number of nodes in a trie
-   c: number of children of a given node

获取搜索次数最多的 k 个查询的步骤如下：

-   Find the prefix. Time complexity: O(p)
-   Traverse the subtree from the prefix node to get all valid children. A child is valid if it can form a valid query string. Time complexity: O(c)
-   Sort the children and get top k. Time complexity: O(clogc)

假设 k 等于 2，用户在搜索框中输入 "tr"

-   Step 1: Find the prefix node “tr”
-   Step 2: Traverse the subtree to get all valid children. In this case, nodes [tree: 10], [true: 35], [try: 29] are valid
-   Step 3: Sort the children and get top 2. [true: 35] and [try: 29] are the top 2 queries with prefix “tr”

![](https://inasa.dev/image/systemdesign/13/8.png)

该算法的时间复杂度是上述每个步骤所用时间的总和: O(p) + O(c) + O(clogc)

上述算法简单明了。但是，由于需要遍历整个三角形才能在最坏的情况下得到前 k 个结果，因此速度太慢。下面是两种优化方法：

-   限制前缀的最大长度
    -   用户很少在搜索框中输入很长的搜索查询。因此，可以说 p 是一个小整数，比如 50。如果我们限制前缀的长度，"查找前缀 "的时间复杂度就可以从 O(p)降低到 O(小常数)，也就是 O(1)
-   缓存每个节点的热门搜索查询
    -   为了避免遍历整个三角形，我们在每个节点上存储最常用的 k 个查询。对用户来说，5 到 10 个自动完成建议就足够了，因此 k 是一个相对较小的数字
    -   通过在每个节点缓存顶级搜索查询，我们大大减少了检索前 5 个查询的时间复杂度。不过，这种设计需要大量空间来存储每个节点的顶级查询。以空间换时间是非常值得的，因为快速响应时间非常重要

![](https://inasa.dev/image/systemdesign/13/9.png)

重新看看应用这两项优化后算法的时间复杂度

-   Find the prefix node. Time complexity: O(1)
-   Return top k. Since top k queries are cached, the time complexity for this step is O(1)

##### Data gathering service

在之前的设计中，每当用户输入搜索查询，数据就会实时更新。由于以下两个原因，这种方法并不实用：

-   用户每天可能输入数十亿次查询。每次查询都更新三元组 会大大降低查询服务的速度
-   一旦建立了三元组，顶部建议可能不会有太大变化。因此，没有必要频繁更新三角模型

为了设计可扩展的数据收集服务，我们要研究数据的来源和使用方式。Twitter 等实时应用需要最新的自动完成建议。然而，许多 Google 关键字的自动完成建议可能每天都不会有太大变化。

尽管使用案例不同，但数据收集服务的基础仍然相同，因为用于构建三元组的数据通常来自分析或日志服务。

![](https://inasa.dev/image/systemdesign/13/10.png)

-   Analytics Logs

    -   它存储搜索查询的原始数据。日志仅为附加数据，不会被编入索引。

    -   ![](https://inasa.dev/image/systemdesign/13/11.png)

        

-   Aggregators

    -   分析日志的大小通常很大，而且数据格式不正确。需要汇总数据，以便系统轻松处理
    -   根据不同的使用情况，可能会以不同的方式汇总数据。对于 Twitter 等实时应用，由于实时结果非常重要，会以较短的时间间隔聚合数据。另一方面，聚合数据的频率较低，例如每周一次，对于许多用例来说可能就足够了。在访谈过程中，验证实时结果是否重要。假设 trie 每周重建一次

-   Aggregated Data

    -   ![](https://inasa.dev/image/systemdesign/13/12.png)
    -   "Time "字段表示一周的开始 表示一周的开始时间。"frequency"字段是该周内相应查询次数的总和。

-   Workers

    -   Worker 是一组定期执行异步工作的服务器。它们构建三元组数据结构并将其存储在三元组数据库中

-   Trie Cache

    -   Trie 缓存是一种分布式缓存系统，可将 trie 保存在内存中，以便快速读取。它每周对 DB进行快照

-   Trie DB

    -   Trie DB 是持久性存储。存储数据有两种选择
        -   文件存储： 由于每周都会构建新的三元组，我们可以定期对其进行快照、序列化，并将序列化数据存储到数据库中。像 MongoDB 这样的文档存储非常适合序列化数据
        -   键值存储 三元组可以通过以下逻辑以散列表的形式表示
            -   三元组中的每个前缀都映射到哈希表中的一个键上
            -   每个三角形节点上的数据映射到哈希表中的一个值

    ![](https://inasa.dev/image/systemdesign/13/13.png)

左边的每个三元组节点都会映射到右边的 <键、值> 对



##### Query service

查询服务直接调用数据库获取前 5 个结果

![](https://inasa.dev/image/systemdesign/13/14.png)

查询服务需要快如闪电的速度。提出以下优化建议

-   AJAX request
    -   对于网络应用程序，浏览器通常会发送 AJAX 请求来获取自动完成结果。AJAX 的主要优点是发送/接收请求/响应时不会刷新整个网页
-   Browser caching
    -   对于许多应用程序来说，自动完成搜索建议在短时间内可能不会有太大变化。因此，自动完成建议可以保存在浏览器缓存中，以便后续请求直接从缓存中获取结果
    -   谷歌搜索引擎使用了相同的缓存机制。可以看到，Google 在浏览器中缓存了 1 小时的结果。请注意：cache-control 中的 "private "表示结果只针对单个用户，不得由共享缓存缓存。"max-age=3600 "表示缓存的有效期为 3600 秒，也就是一小时
    -   ![](https://inasa.dev/image/systemdesign/13/15.png)
-   Data sampling
    -   对于大型系统来说，记录每次搜索查询都需要大量的处理能力和存储空间。数据取样非常重要。例如，系统只记录每 N 个请求中的 1 个

##### Trie operations

-   Create
    -   Trie 由workers使用汇总数据创建。数据源来自分析日志/数据库。
-   Update
    -   更新trie有两种方法
        -   每周更新trie。一旦创建了新的trie，新的trie就会取代旧的trie
        -   直接更新单个trie
            -   尽量避免这种操作，因为它很慢
            -   不过，如果trie的规模较小，这也是一种可以接受的解决方案
            -   ![](https://inasa.dev/image/systemdesign/13/16.png)
-   Delete
    -   必须删除仇恨、暴力、色情或危险的自动完成建议
    -   在 Trie 缓存前添加了一个过滤层，以过滤掉不需要的建议。有了过滤层，就可以根据不同的过滤规则灵活移除结果。不需要的建议会以异步方式从数据库中物理移除，以便在下一个更新周期中使用正确的数据集来构建 trie
    -   ![](https://inasa.dev/image/systemdesign/13/17.png)

##### Scale the storage

由于英语是唯一支持的语言，因此根据第一个字符进行分片是一种简单的方法。

-   如果需要两台服务器来存储，我们可以在第一台服务器上存储以 "a "到 "m "开头的查询，在第二台服务器上存储以 "n "到 "z "开头的查询
-   如果需要三个服务器，我们可以将查询分为 "a "至 "i"、"j "至 "r "和 "s "至 "z"。

根据这一逻辑，可以将查询分到 26 个服务器，因为英语中有 26 个字母字符。把基于第一个字符的分片定义为第一级分片。要存储超过 26 台服务器的数据，可以在第二级甚至第三级进行分片。例如，以 "a "开头的数据查询可以分成 4 个服务器："asa-ag"、"ah-an"、"ao-au "和 "av-az"。

乍一看，这种方法似乎很合理，直到意识到以字母 "c "开头的单词比以 "x "开头的单词多得多。这就造成了分布不均。

为缓解数据不平衡问题，分析了历史数据的分布模式，并应用了更智能的分片逻辑

分片映射管理器维护着一个查找数据库，用于确定行应存储在哪里。例如，如果 "s "的历史查询次数与 "u"、"v"、"w"、"x"、"y "和 "z "的历史查询次数相近，我们可以维护两个分片：一个用于 "s"，另一个用于 "u "至 "z"。

![](https://inasa.dev/image/systemdesign/13/18.png)

#### Step 4 - Wrap up

Interviewer: How do you extend your design to support multiple languages?

为了支持其他非英语查询，将 Unicode 字符存储在 trie 节点中。



Interviewer: What if top search queries in one country are different from others?

在这种情况下，可能会为不同的国家建立不同的尝试。为了缩短响应时间，可以将尝试存储在 CDN 中。



Interviewer: How can we support the trending (real-time) search queries?

假设有新闻事件发生，搜索查询突然变得流行起来。我们原来的设计就会失效，因为

-   offline workers尚未计划更新 trie，因为计划每周运行一次
-   即使排定了时间，也需要花费太长时间来构建trie

构建实时搜索自动完成功能非常复杂，超出了本书的讨论范围，因此只给出一些想法：

-   通过分片减少工作数据集
-   更改排名模型，增加最近搜索查询的权重
-   数据可能以数据流的形式出现，因此无法一次性获取所有数据
-   流数据是指连续生成的数据。流处理需要一套不同的系统： Apache Hadoop MapReduce、Apache Spark Streaming、Apache Storm、Apache Kafka 等