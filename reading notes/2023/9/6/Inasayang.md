### CHAPTER 8: DESIGN A URL SHORTENER

#### Step 1 - Understand the problem and establish design scope

-   Candidate: 能举例说明 URL 缩短器是如何工作的吗？
-   Interviewer: 假设 URL https://www.systeminterview.com/q=chatsystem&c=loggedin&v=v3&l=long 是原始URL。您的服务会创建一个长度更短的别名：https://tinyurl.com/y7keocwj。点击别名，就会重定向到原始 URL
-   Candidate:  流量有多大？
-   Interviewer: 每天产生 1 亿个 URL
-   Candidate:  缩短后的 URL 有多长？
-   Interviewer: 越短越好
-   Candidate:  缩短后的 URL 允许包含哪些字符？
-   Interviewer: 可以是数字（0-9）和字符（a-z、A- Z)
-   Candidate:  缩短的 URL 是否可以删除或更新？
-   Interviewer: 假设缩短的 URL 不能删除或更新



##### Back of the envelope estimation

-   Write operation: 100 million URLs are generated per day
-   Write operation per second: 100 million / 24 /3600 = 1160
-   Read operation: Assuming ratio of read operation to write operation is 10:1, read operation per second: 1160 * 10 = 11,600
-   Assuming the URL shortener service will run for 10 years, this means we must support 100 million * 365 * 10 = 365 billion records
-   Assume average URL length is 100
-   Storage requirement over 10 years: 365 billion * 100 bytes * 10 years = 365 TB



#### Step 2 - Propose high-level design and get buy-in

##### API Endpoints

API Endpoints可促进客户端与服务器之间的通信。

URL缩短器主要需要两个API端点

-   URL shortening
    -   要创建一个新的短 URL，客户端需要发送一个 POST 请求，其中包含一个参数：原始长 URL
    -   POST `api/v1/data/shorten`\
        -    `request parameter: {longUrl: longURLString}` 
        -   `return shortURL`
-   URL redirecting
    -   要将短 URL 重定向到相应的长 URL，客户端需要发送一个 GET 请求
    -   GET `api/v1/shortUrl`
        -   `Return longURL for HTTP redirection`





##### URL redirecting

![](https://inasa.dev/image/systemdesign/08/1.png)

一旦服务器收到 `tinyurl` 请求后，就会通过 301 重定向将短 URL 更改为长 URL

![](https://inasa.dev/image/systemdesign/08/2.png)



-   **301 redirect**
    -   301 重定向表示请求的 URL 已 "永久 "转移到长 URL。由于是永久重定向，浏览器会缓存响应，对同一 URL 的后续请求将不会发送到 URL 缩短服务。相反，请求会直接重定向到长 URL 服务器。
-   **302 redirect**
    -   302 重定向意味着 URL 被 "临时 "转移到长 URL，这意味着对同一 URL 的后续请求将首先发送到 URL 缩短服务。然后，它们会被重定向到长 URL 服务器。

每种重定向方法都各有利弊。如果优先考虑的是减少服务器负载，那么使用 301 重定向是合理的，因为只有相同 URL 的第一次请求会被发送到 URL 缩短服务器。但是，如果分析很重要，302 重定向则是更好的选择，因为它可以更轻松地跟踪点击率和点击来源。

实现 URL 重定向的最直观方法是使用哈希表。假设哈希表存储的是 `<shortURL, longURL>` 对



##### URL shortening

![](https://inasa.dev/image/systemdesign/08/3.png)

-   Each longURL must be hashed to one hashValue
-   Each hashValue can be mapped back to the longURL



#### Step 3 - Design deep dive

##### Data model

在高层设计中，所有内容都存储在哈希表中。这是一个很好的起点，但由于内存资源有限且昂贵，这种方法在实际系统中并不可行。更好的办法是将 `<shortURL, longURL>` 映射存储在关系数据库中。

![](https://inasa.dev/image/systemdesign/08/4.png)



##### Hash function

###### Hash value length

hashValue 由 [0-9、a-z、A-Z] 中的字符组成，包含 10 + 26 + 26 = 62 个可能的字符。

要计算 hashValue 的长度，需要找出最小的 n，使得 62^n ≥ 3650 亿。

![](https://inasa.dev/image/systemdesign/08/5.png)

当 n = 7 时，62 ^ n = ~3.5 万亿，3.5 万亿足以容纳 365 亿个 URL、 因此，hashValue 的长度为 7。



探讨两种适用于 URL 缩短器的哈希函数。

-   第一种是 "Hash + collision resolution"
-   第二种是 "Base 62 conversion"

###### Hash + collision resolution

为了缩短长 URL 的长度，我们应该实现一个散列函数，将长 URL 散列为 7 个字符的字符串。一个直接的解决方案是使用众所周知的哈希函数，如 CRC32、MD5 或 SHA-1。

![](https://inasa.dev/image/systemdesign/08/6.png)

第一种方法是收集哈希值的前 7 个字符，但这种方法可能会导致哈希碰撞。为了解决哈希碰撞，我们可以递归地追加一个新的预定义字符串，直到不再发现碰撞为止。

![](https://inasa.dev/image/systemdesign/08/7.png)



这种方法可以消除碰撞；但是，查询数据库以检查每个请求是否存在短网址的成本很高。每个请求是否存在 shortURL 的成本很高。可以使用布隆过滤器



###### Base 62 conversion

基数转换是 URL 捷径常用的另一种方法。基数转换有助于在不同的数字表示系统之间转换相同的数字。使用基数 62 转换是因为哈希值有 62 个可能的字符。让我们举例说明转换的原理：将 11157~10~ 转换为基数 62 表示法

11157~10~ represents 11157 in a base 10 system

-   From its name, base 62 is a way of using 62 characters for encoding. The mappings are: 0-0, ..., 9-9, 10-a, 11-b, ..., 35-z, 36-A, ..., 61-Z, where ‘a’ stands for 10, ‘Z’ stands for 61, etc.
-   11157~10~ = 2 x 62^2^ + 55 x 62^1^+ 59 x 62^0^ = [2, 55, 59] -> [2, T, X] in base 62

![](https://inasa.dev/image/systemdesign/08/8.png)

the short URL is https://tinyurl.com/2TX



###### Comparison of the two approaches

![](https://inasa.dev/image/systemdesign/08/9.png)



##### URL shortening deep dive

![](https://inasa.dev/image/systemdesign/08/10.png)



![](https://inasa.dev/image/systemdesign/08/11.png)

值得一提的是分布式唯一 ID 生成器。它的主要功能是生成全局唯一 ID，用于创建 shortURL。

##### URL redirecting deep dive

![](https://inasa.dev/image/systemdesign/08/12.png)



#### Step 4 - Wrap up

额外的谈话要点

-   Rate limiter
    -   恶意用户会发送大量的 URL 缩短请求。速率限制器有助于根据 IP 地址或其他过滤规则过滤掉请求
-   Web server scaling
    -   由于网络层是无状态的，因此很容易通过添加或删除网络服务器来扩展网络层
-   Database scaling
-   Analytics
    -   数据对于企业的成功越来越重要。将分析解决方案集成到 URL 简化器中，有助于回答一些重要问题，如多少人点击链接？他们何时点击链接？
-   Availability, consistency, and reliability



Pp. 119-131