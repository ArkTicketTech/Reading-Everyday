### CHAPTER 7: DESIGN A UNIQUE ID GENERATOR IN DISTRIBUTED SYSTEMS

#### Step 1 - Understand the problem and establish design scope

-   Candidate: 唯一 ID 有哪些特征？
-   Interviewer: 唯一且可分类
-   Candidate: 对于每条新记录，ID 是否递增 1？
-   Interviewer: ID 按时间递增，但不一定只按 1 递增。在同一天，晚上创建的 ID 要大于早上创建的 ID。
-   Candidate: ID 是否只包含数值？
-   Interviewer: 是的
-   Candidate: ID 长度要求是多少？
-   Interviewer: 64位
-   Candidate: 系统的规模有多大？
-   Interviewer: 每秒应能生成 10000 个 ID

requirements

-   unique
-   numerical values only
-   64-bit
-    ordered by date
-   generate over 10,000 unique IDs per second



#### Step 2 - Propose high-level design and get buy-in

已有方案

-   Multi-master replication
-   Universally unique identifier (UUID)
-   Ticket server
-   Twitter snowflake approach

##### Multi-master replication

![](https://inasa.dev/image/systemdesign/07/1.png)

这种方法使用了数据库的自动增加功能

不是将下一个 ID 增加 1，而是增加 k，其中 k 是正在使用的数据库服务器的数量。

如图所示，要生成的下一个 ID 等于同一服务器中上一个 ID 加 2。这解决了一些可扩展性问题，因为 ID 可以随着数据库服务器数量的增加而增加。

缺陷：

-   难以通过多个数据中心进行扩展
-   在多个服务器上，ID 不会随时间而增加
-   当增加或删除服务器时，它不能很好地扩展



##### UUID

UUID 是获取唯一 ID 的另一种简便方法

UUID 是一个 128 位的数字，用于识别计算机系统中的信息

UUID 被串通的概率非常低

>   引自维基百科："每秒生成 10 亿个 UUID，持续约 100 年后，创建一个重复的概率将达到 50%"

![](https://inasa.dev/image/systemdesign/07/2.png)

UUID 可以独立生成，服务器之间无需协调

每个网络服务器都包含一个 ID 生成器，网络服务器负责独立生成 ID

Pros

-   生成 UUID 非常简单。服务器之间无需协调，因此不会出现任何同步问题
-   该系统易于扩展，因为每个网络服务器都负责生成它们使用的 ID。ID 生成器可以很容易地随着网络服务器的扩展而扩展

Cons

-   ID 长度为 128 位，但我们的要求是 64 位
-   ID 不会随着时间的推移而增加
-   ID 可以是非数字



##### Ticket Server

![](https://inasa.dev/image/systemdesign/07/3.png)

在单个数据库服务器（Ticket Server）中使用集中式自动增加功能

Pros

-   数字ID
-   易于实施，适用于中小型应用程序

Cons

-   单点故障。单一票务服务器意味着如果票务服务器出现故障，所有依赖于它的系统都将面临问题。为了避免单点故障，可以设置多个票务服务器。然而，这将带来新的挑战，如数据同步



##### Twitter snowflake approach

不直接生成 ID，而是将 ID 分成不同的部分

![](https://inasa.dev/image/systemdesign/07/4.png)

-   符号位： 1 位。始终为 0。保留给将来使用。有可能用于区分有符号和无符号数字。
-   时间戳： 41 位。自纪元或自定义纪元起的毫秒数。使用 1288834974657，相当于Nov 04, 2010, 01:42:54 UTC
-   据中心 ID：5 位，即 2 ^ 5 = 32 个数据中心
-   机器 ID：5 位，即每个数据中心有 2 ^ 5 = 32 台机器
-   序列号：12 位。该机器/进程每生成一个 ID，序列号就增加 1，每毫秒重置为 0



Pp. 110-115