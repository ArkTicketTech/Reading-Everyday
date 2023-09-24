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