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