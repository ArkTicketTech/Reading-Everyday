### Chapter 2：back-of-the-envelope estimation事后估算

在系统设计面试中，有时会要求使用 "`back-of-the-envelope estimation`"估算系统容量或性能要求。

 应充分理解以下概念：2 的幂次、延迟数字以及可用性数字

#### Power of two

略

#### Latency numbers

随着计算机速度越来越快，功能越来越强大，有些数字已经过时。不过，这些数字仍能让我们了解不同计算机操作的快慢。

![](https://inasa.dev/image/systemdesign/02/1.png)

![](https://inasa.dev/image/systemdesign/02/2.png)

- 内存速度快，但磁盘速度慢
- 尽可能避免磁盘寻道
- 简单的压缩算法速度很快
- 尽可能在通过互联网发送数据前对其进行压缩
- 数据中心通常位于不同地区，在它们之间发送数据需要时间



#### Availability numbers

高可用性是指系统在理想的长时间内持续运行的能力。高可用性用百分比来衡量，100% 表示服务 0 停机时间。大多数服务介于 99% 和 100% 之间。

`service level agreement` 服务水平协议（`SLA`）是服务提供商的常用术语。这是服务提供商与客户之间的协议，该协议正式规定了服务的正常运行时间。云服务提供商亚马逊, 谷歌和微软将其 `SLA` 定义为 `99.9%` 或以上。正常运行时间传统上以 9 为单位。9 越多越好。如表 2-3 所示，9 的数量与预期的系统正常运行时间相关。与预期的系统停机时间相关。

![](https://inasa.dev/image/systemdesign/02/3.png)



#### Example: Estimate Twitter（X） QPS and storage requirements

Assumptions

-   300 million monthly active users
-   50% of users use Twitter daily
-   Users post 2 tweets per day on average
-   10% of tweets contain media
-   Data is stored for 5 years

Estimations

-   Daily active users (DAU) = 300 million * 50% = 150 million
-   Tweets QPS = 150 million * 2 tweets / 24 hour / 3600 seconds = ~3500
-   Peek QPS = 2 * QPS = ~7000
    -   系统在特定时间段内（通常是一秒钟）所能达到的最高查询率（Queries Per Second，每秒查询数）
    -   为什么假设是2倍，不知道...

only estimate media storage here

-   Average tweet size
    -   tweet_id 64 bytes
    -   text 140 bytes
    -   media 1 MB
-   Media storage: 150 million * 2 * 10% * 1 MB = 30 TB per day
-   5-year media storage: 30 TB * 365 * 5 = ~55 PB



#### Tips

-   Rounding and Approximation
    -   四舍五入和近似计算。在面试中很难进行复杂的数学运算 在面试过程中很难进行复杂的数学运算。例如，"99987 / 9.1 "的结果是什么？没有必要 没有必要花费宝贵的时间来解决复杂的数学问题。不要求精确。使用四舍五入法和近似法。除法问题可简化如下 简化如下 "100,000 / 10"
-   Write down your assumptions
    -   写下假设。最好写下假设，以便日后参考
-   Label your units
    -   标注单位。当写下 "5 "时，是指 5 KB 还是 5 MB？可能会因此而困惑。写下单位是因为 "5 MB "有助于消除歧义
-   Commonly asked back-of-the-envelope estimations
    -   经常被问及的包络式估算： QPS、峰值 QPS、存储、缓存、服务器数量等

Pp. 34-41