### CHAPTER 14: DESIGN YOUTUBE

YouTube 看起来很简单：内容创作者上传视频，观众点击播放。真的这么简单吗？其实不然。在简单的背后有许多复杂的技术。看看 2020 年 YouTube 的一些令人印象深刻的统计数据、人口统计和趣事

-   每月活跃用户总数：20 亿
-   每天观看的视频数量 50 亿次
-   73% 的美国成年人使用 YouTube
-   YouTube 上有 5000 万创作者
-   2019 年全年，YouTube 的广告收入为 151 亿美元，比 2018 年增长 36%
-   YouTube 占所有移动互联网流量的 37%
-   YouTube 有 80 种不同语言



#### Step 1 - Understand the problem and establish design scope

-   Candidate: 哪些功能很重要？
-   Interviewer: 能够上传视频和观看视频
-   Candidate: 需要支持哪些客户端？
-   Interviewer: Mobile apps, web browsers, and smart TV
-   Candidate: 有多少日活跃用户？
-   Interviewer: 5 million
-   Candidate: 每天平均使用该产品的时间是多少？
-   Interviewer: 30 minutes
-   Candidate: 需要支持国际用户吗？
-   Interviewer: 是的，很大一部分用户是国际用户
-   Candidate: 支持哪些视频分辨率？
-   Interviewer: 系统可接受大多数视频分辨率和格式
-   Candidate: 是否需要加密？
-   Interviewer: 是
-   Candidate: 对视频文件大小有要求吗？
-   Interviewer: 平台专注于中小型视频。最大允许的最大视频大小为 1GB
-   Candidate: 能否利用亚马逊、谷歌或微软提供的一些现有云基础设施？
-   Interviewer: 这是一个很好的问题。对于大多数公司来说，从零开始构建一切是不现实的，建议利用现有的一些云服务



将重点设计具有以下功能的视频流媒体服务：

-   快速上传视频的能力
-   流畅的视频流
-   可改变视频质量
-   基础设施成本低
-   高可用性、可扩展性和可靠性要求
-   支持的客户端：移动应用程序、网络浏览器和智能电视

##### Back of the envelope estimation

-   假设产品拥有 500 万日活跃用户
-   用户每天观看 5 个视频
-   10% 的用户每天上传 1 个视频
-   假设视频的平均大小为 300 MB
-   每天需要的总存储空间： 500 万 * 10% * 300 MB = 150TB
-   CDN 成本
    -   当 CDN 提供视频服务时，需要支付从 CDN 传输出来的数据费用
    -   假设 100% 的流量来自美国。每 GB 平均成本为 0.02 美元。为简单起见，只计算视频流的成本
    -   500 万 * 5 个视频 * 0.3GB * 0.02 美元 = 每天 15 万美元

通过粗略的成本估算，知道从 CDN 提供视频需要花费大量资金。尽管云提供商愿意为大客户大幅降低 CDN 成本，但成本仍然不菲。



#### Step 2 - Propose high-level design and get buy-in

面试官建议利用现有的云服务，而不是从头开始构建一切。CDN 和 blob 存储就是要利用的云服务。

为什么不自己构建一切呢？

-   系统设计面试并不是要从头开始构建一切。在有限的时间内，选择正确的技术来完成工作比详细解释技术的工作原理更重要。例如，面试时提到用于存储源视频的 blob 存储就足够了。谈论 blob 存储的详细设计 blob存储的详细设计可能会矫枉过正。
-   构建可扩展的 Blob 存储或 CDN 极其复杂且成本高昂。即使是 Netflix 或 Facebook 这样的大公司也不会自己构建一切。Netflix 利用亚马逊的云服务，而 Facebook 则使用 Akamai 的 CDN。



在高层次上，该系统包括三个组成部分

![](https://inasa.dev/image/systemdesign/14/1.png)

-   Client: 可以在电脑、手机和智能电视上观看 YouTube
-   CDN: 视频存储在 CDN 中。按下播放键后，视频就会从 CDN 流式传输
-   API servers: 除视频流外，其他一切都通过 API 服务器进行。这包括feed recommendation、生成视频上传 URL、更新元数据数据库和缓存、用户注册等



在问答环节，面试官对两个流程表现出了兴趣：

-   Video uploading flow
-   Video streaming flow