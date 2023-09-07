### CHAPTER 9: DESIGN A WEB CRAWLER

![](https://inasa.dev/image/systemdesign/09/1.png)

用途

-   Search engine indexing
-   Web archiving
-   Web mining
-   Web monitoring



#### Step 1 - Understand the problem and establish design scope

-   Candidate: 爬虫的主要用途是什么？是用于搜索引擎索引、数据挖掘还是其他？
-   Interviewer: 搜索引擎索引
-   Candidate:  每月收集多少网页？
-   Interviewer:  10 亿页
-   Candidate:  包括哪些内容类型？只包括 HTML 还是也包括 PDF 和图片等其他内容类型？
-   Interviewer:  HTML only
-   Candidate:  是否应考虑新添加或编辑的网页？
-   Interviewer:  是的
-   Candidate:  需要存储从网上抓取的 HTML 网页吗？
-   Interviewer:  是，最长 5 年
-   Candidate:  如何处理内容重复的网页？
-   Interviewer:  忽略



-   Scalability
-   Robustness
    -   必须处理所有边缘情况（坏的 HTML、反应迟钝的服务器、崩溃、恶意链接等）
-   Politeness
    -   不应在短时间内向网站发出过多请求
-   Extensibility



##### Back of the envelope estimation

-   Assume 1 billion web pages are downloaded every month
-   QPS: 1,000,000,000 / 30 days / 24 hours / 3600 seconds = ~400 pages per second
-   Peak QPS = 2 * QPS = 800
-   Assume the average web page size is 500k
-   1-billion-page x 500k = 500 TB storage per month
-   Assuming data are stored for five years, 500 TB * 12 months * 5 years = 30 PB. A 30 PB storage is needed to store five-year content



#### Step 2 - Propose high-level design and get buy-in

![](https://inasa.dev/image/systemdesign/09/2.png)



##### Seed URLs

一个好的种子网址是一个良好的起点，爬虫可以利用它尽可能多地遍历链接

一般策略是将整个 URL 空间划分为更小的空间。首先提出的方法是基于地域性，因为不同国家可能有不同的流行网站

另一种方法是根据主题选择种子 URL，例如，我们可以将 URL 空间划分为购物、体育、医疗保健等

种子网址选择是一个开放式问题



##### URL Frontier

大多数现代网络爬虫将抓取状态分为两种：待下载和已下载

存储待下载 URL 的组件称为 URL 前沿，可以将其称为先进先出（FIFO）队列



##### HTML Downloader

HTML 下载器可从互联网上下载网页。这些 URL 由 URL Frontier 提供

##### DNS Resolver

HTML下载器会调用 DNS 解析器来获取 URL 对应的 IP 地址

##### Content Parser

下载网页后，必须对其进行解析和验证，因为畸形网页可能会引发问题并浪费存储空间。在抓取服务器中实施内容解析器会减慢抓取过程。因此，内容解析器是一个独立的组件

##### Content Seen?

在线调查显示，29% 的网页内容重复，这可能导致同一内容被存储多次

消除数据冗余并缩短处理时间, 有助于检测之前存储在系统中的新内容

要比较两个 HTML 文档，我们可以逐个字符进行比较。然而，这种方法既慢又耗时，尤其是在涉及数十亿网页的情况下。完成这项任务的有效方法是比较两个网页的哈希值



##### Content Storage

存储系统的选择取决于数据类型、数据大小、访问频率、寿命等因素。磁盘和内存均可使用



##### URL Extractor

解析并提取 HTML 网页中的链接

![](https://inasa.dev/image/systemdesign/09/3.png)



##### URL Filter

排除某些内容类型、文件扩展名、错误链接和 "黑名单 "网站中的 URL

##### URL Seen?

"URL Seen? "是一种数据结构，用于跟踪之前访问过或已在前沿中访问过的 URL

有助于避免多次添加相同的 URL，因为这会增加服务器负载并导致潜在的无限循环

Bloom 过滤器和哈希表是实现 "URL Seen? "组件的常用技术

##### URL Storage

存储已访问过的 URL

##### Web crawler workflow

![](https://inasa.dev/image/systemdesign/09/4.png)



#### Step 3 - Design deep dive

-   Depth-first search (DFS) vs Breadth-first search (BFS)

    -   两种常见的图遍历算法是 DFS 和 BFS。然而，DFS 通常不是一个好的选择，因为 DFS 的深度可能非常深

    -   BFS 常用于网络爬虫，由先进先出队列（FIFO）实现。在先进先出队列中，URL 按照入列顺序被排出。然而，这种实现方式有两个问题：

        -   来自同一网页的大多数链接都会链接回同一主机

            -   wikipedia.com 中的所有链接都是内部链接，这使得爬虫忙于处理来自同一主机的 URL。当爬虫试图并行下载网页时，维基百科服务器将被大量请求淹没。这被视为 "不礼貌"。

                ![](https://inasa.dev/image/systemdesign/09/5.png)

            -   标准 BFS 不考虑 URL 的优先级。网络很大，并非每个页面的质量和重要性都相同。因此，我们可能需要根据页面排名、网络流量、更新频率等因素来确定 URL 的优先级

-   URL frontier

    -   Politeness

        -   在没有任何限制的情况下，爬虫可以每秒向同一个网站发送数千个请求。这会使网络服务器不堪重负

        -   执行 "礼貌 "的总体思路是从同一主机一次下载一个页面。两个下载任务之间可以增加延迟。通过维护网站主机名与下载（工作）线程之间的映射关系，可以实现礼貌约束。每个下载线程都有一个独立的先进先出队列，只能下载从该队列中获取的 URL

            ![](https://inasa.dev/image/systemdesign/09/6.png)

            -   Queue router: 它确保每个队列（b1、b2、......bn）只包含来自同一主机的 URL。同一主机的 URL

            -   Mapping table: 它将每个主机映射到一个队列

                ![](https://inasa.dev/image/systemdesign/09/7.png)

            -   FIFO queues b1, b2 to bn: 每个队列都包含来自同一主机的 URL

            -   Queue selector: 每个工作线程都映射到一个先进先出队列，并且只从该队列下载 URL。队列选择逻辑由队列选择器完成

            -   Worker thread 1 to N:  工作线程从同一主机逐个下载网页。两个下载任务之间可以增加延迟

    -   Priority

        -   在苹果产品论坛上随意发布的帖子与在苹果主页上发布的帖子的权重是完全不同的。尽管两者都有 "Apple "关键词，但爬虫还是会首先抓取苹果主页

        -   根据有用性对 URL 进行优先排序，有用性可以用 PageRank、网站流量、更新频率等来衡量。"优先级排序器 "是处理 URL 优先级排序的组件

            ![](https://inasa.dev/image/systemdesign/09/8.png)

            -   Prioritizer: 将 URL 作为输入，并计算优先级

            -   Queue f1 to fn: 每个队列都有指定的优先级。优先级高的队列被选中的概率更高

            -   Queue selector: 随机选择一个队列，偏向于优先级较高的队列

                ![](https://inasa.dev/image/systemdesign/09/9.png)

    -   Freshness

        -   网页不断被添加、删除和编辑。网络爬虫必须定期重新抓取下载的网页，以保持数据集的新鲜度。重新抓取所有 URL 既耗时又耗资源
        -   基于网页更新历史的重新抓取
        -   对 URL 进行优先排序，更频繁地首先重新抓取重要页面

    -   Storage for URL Frontier

        -   在搜索引擎的实际抓取过程中，前沿 URL 的数量可能高达数亿。把所有内容都放在内存中既不耐用，也无法扩展。将所有内容保存在磁盘中也不可取，因为磁盘速度很慢，而且很容易成为抓取的瓶颈
        -   大多数 URL 都存储在磁盘上，因此存储空间不成问题。为了降低从磁盘读取数据和向磁盘写入数据的成本，我们在内存中保留了缓冲区，用于执行 "排队"/"取消排队 "操作。缓冲区中的数据 定期写入磁盘

-   HTML Downloader

    -   Robots.txt

        -   Robots.txt 全称为《机器人排除协议》（Robots Exclusion Protocol），是网站用于与爬虫通信的标准。它规定了允许爬虫下载的页面。在尝试抓取网站之前，爬虫应首先检查相应的 robots.txt，并遵守其规则

        -   为避免重复下载 robots.txt 文件，我们对文件结果进行了缓存。文件 定期下载并保存到缓存中。下面是一段 robots.txt 文件，摘自 https://www.amazon.com/robots.txt。一些目录（如 creatorhub）不允许谷歌机器人

            ```
            User-agent: Googlebot
            Disallow: /creatorhub/*
            Disallow: /rss/people/*/reviews
            Disallow: /gp/pdp/rss/*/reviews
            Disallow: /gp/cdp/member-reviews/
            Disallow: /gp/aw/cr/
            ```

    -   Performance optimization

        -   Distributed crawl
            -   为了实现高性能，抓取工作被分配到多个服务器上，每个服务器运行多个线程。URL 空间被分割成更小的片段；因此，每个下载器负责 URL 的一个子集
        -   Cache DNS Resolver
            -   DNS 解析器是爬虫的一个瓶颈，因为许多 DNS 接口都是同步的，所以 DNS 请求可能需要一些时间。DNS 响应时间从 10 毫秒到 200 毫秒不等。一旦爬虫线程向 DNS 发出请求，其他线程就会被阻塞，直到第一个请求完成。维护 DNS 缓存以避免频繁调用 DNS 是一项有效的速度优化技术。我们的 DNS 缓存保存域名到 IP 地址的映射，并通过 cron 作业定期更新
        -   Locality
            -   按地域分布抓取服务器。当抓取服务器离网站主机更近时，抓取程序的下载时间会更快。本地化设计适用于大多数系统组件：抓取服务器、缓存、队列、存储等
        -   Short timeout
            -   有些网络服务器响应缓慢，甚至根本不响应。为避免等待时间过长，可指定一个最大等待时间。如果主机在预定时间内没有响应，爬虫将停止工作，转而抓取其他网页。爬虫就会停止工作并抓取其他网页

-   Robustness

    -   Consistent hashing
        -   有助于在下载者之间分配负载。可以使用一致散列添加或删除新的下载服务器
    -   Save crawl states and data
        -   为防止出现故障，爬行状态和数据会被写入存储系统。通过加载保存的状态和数据，可以很容易地重新启动中断的爬行
    -   Exception handling
        -   在大型系统中，错误是不可避免的，也是常见的。爬虫必须从容应对异常，而不会导致系统崩溃
    -   Data validation
        -   防止系统出错的重要措施

-   Extensibility

    -   几乎每个系统都在不断发展，设计目标之一就是使系统足够灵活，以支持新的内容类型。爬虫可以通过插入新模块进行扩展

        ​	![](https://inasa.dev/image/systemdesign/09/11.png)

-   Detect and avoid problematic content

    -   检测和防止多余、无意义或有害的内容
    -   Redundant content
        -   近 30% 的网页是重复的。哈希值或校验和有助于检测重复
    -   Spider traps
        -   蜘蛛陷阱是指会导致爬虫无限循环的网页。例如，一个无限深的目录结构如下：www.spidertrapexample.com/foo/bar/foo/bar/foo/bar/…
        -   可以通过设置 URL 的最大长度来避免这种蜘蛛陷阱。然而，并不存在一种万能的解决方案来检测蜘蛛陷阱。含有蜘蛛陷阱的网站 由于在这类网站上发现的网页数量异常庞大，因此很容易识别出含有蜘蛛陷阱的网站。很难开发出避免蜘蛛陷阱的自动算法；不过，用户可以手动验证和识别蜘蛛陷阱，并将这些网站从爬虫中排除，或应用一些定制的 URL 过滤器
    -   Data noise
        -   有些内容几乎没有价值，如广告、代码片段、垃圾 URL 等。这些内容对爬虫没有用处，应尽可能排除在外



#### Step 4 - Wrap up

遗漏了很多相关的讨论要点

-   Server-side rendering
    -   许多网站使用 JavaScript、AJAX 等脚本来动态生成链接。如果我们直接下载和解析网页，就无法检索动态生成的链接。为了解决这个问题，在解析网页之前首先执行服务器端呈现（也称为动态呈现）
-   Filter out unwanted pages
    -   过滤掉不需要的网页 在存储容量和抓取资源有限的情况下，反垃圾组件有利于过滤掉低质量和垃圾网页 
-   Database replication and sharding
-   Horizontal scaling
    -   对于大规模抓取，需要数百甚至数千台服务器来执行下载任务。关键是要保持服务器无状态
-   Availability, consistency, and reliability
-   Analytics
    -   收集和分析数据是任何系统的重要组成部分，因为数据是微调的关键要素。



Pp. 132-150