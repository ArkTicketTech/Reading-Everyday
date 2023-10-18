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

##### Video uploading flow

![](https://inasa.dev/image/systemdesign/14/2.png)

由以下部分组成：

-   User
-   Load balancer
-   API servers
    -   除视频流外，所有用户请求均通过 API 服务器处理
-   Metadata DB
    -   视频元数据存储在元数据DB 中。它采用分片和复制方式，以满足性能和高可用性要求
-   Metadata cache
    -   缓存视频元数据和用户对象，以提高性能
-   Original storage
    -   blob 存储系统用于存储原始视频
    -   二进制大对象（BLOB）是数据库管理系统中作为单一实体存储的二进制数据集合
-   Transcoding servers
    -   视频转码也称为视频编码。它是将一种视频格式转换为其他格式（MPEG、HLS 等）的过程，这些格式可为不同的设备和带宽能力提供最佳的视频流
-   Transcoded storage
    -   它是一个 blob 存储器，用于存储已转码的视频文件
-   CDN
-   Completion queue
    -   它是一个消息队列，用于存储有关视频转码完成事件的信息
-   Completion handler
    -   这包括从完成队列中提取事件数据并更新元数据缓存和数据库的工作程序列表

在了解了每个组件之后，来看看视频上传流程是如何工作的。该流程分为两个并行运行的过程。

-   Upload the actual video
-   Update video metadata. Metadata contains information about video URL, size, resolution, format, user info, etc

##### upload the actual video

![](https://inasa.dev/image/systemdesign/14/3.png)

-   视频上传到原始存储空间
-   转码服务器从原始存储器中获取视频并开始转码
-   转码完成后，将并行执行以下两个步骤
    -   转码后的视频被发送到转码存储区
    -   转码完成事件在完成队列中排队
-   转码视频分发到 CDN
-   完成处理程序包含一堆工作程序，它们会不断从队列中提取事件数据
-   视频转码完成后，完成处理程序会更新元数据数据库和缓存
-   应用程序接口服务器通知客户端，视频已成功上传并可用于流媒体播放

##### update the metadata

在文件上传到原始存储空间的同时，客户端会并行发送更新视频元数据的请求。该请求包含视频元数据，包括文件名、大小、格式等。API 服务器更新元数据缓存和数据库。

![](https://inasa.dev/image/systemdesign/14/4.png)

##### Video streaming flow

在 YouTube 上观看视频时，通常会立即开始流式传输，而不会等到整个视频下载完毕。下载指的是将整个视频复制到你的设备上，而串流指的是设备不断接收来自远程视频源的视频流。观看流媒体视频时，客户端每次只加载一点数据，因此可以立即连续观看视频。

先了解一个重要的概念：流协议。这是一种控制视频流数据传输的标准化方式。流行的流协议有

-   MPEG–DASH. MPEG stands for “Moving Picture Experts Group” and DASH stands for "Dynamic Adaptive Streaming over HTTP"
-   Apple HLS. HLS stands for “HTTP Live Streaming”
-   Microsoft Smooth Streaming
-   Adobe HTTP Dynamic Streaming (HDS)

不需要完全理解甚至记住这些流媒体协议名称，因为它们是需要特定领域知识的低级细节。重要的是要了解不同的流媒体协议支持不同的视频编码和播放器。在设计视频流服务时，必须选择正确的流协议来支持用例。

视频直接从 CDN 流式传输。离您最近的边缘服务器将提供视频。因此，延迟非常小。

![](https://inasa.dev/image/systemdesign/14/5.png)

#### Step 3 - Design deep dive

##### Video transcoding

录制视频时，设备（通常是手机或相机）会为视频文件提供特定的格式。如果想让视频在其他设备上流畅播放，就必须将视频编码成兼容的比特率和格式。比特率是比特在一段时间内的处理速度。更高的比特率通常意味着更高的视频质量。高比特率流需要更强的处理能力和更快的网速。

视频转码非常重要，原因如下：

-   原始视频会消耗大量存储空间。以每秒 60 帧的速度录制一个小时的高清视频可能会占用几百 GB 的空间
-   许多设备和浏览器只支持特定类型的视频格式。因此，出于兼容性考虑，必须将视频编码为不同的格式
-   为确保用户在观看高质量视频的同时保持流畅播放，最好向网络带宽高的用户提供分辨率较高的视频，向带宽低的用户提供分辨率较低的视频
-   网络条件可能会发生变化，尤其是在移动设备上。为确保连续播放视频，必须根据网络条件自动或手动切换视频质量，以获得流畅的用户体验

编码格式有很多种，但大多数都包含两个部分

-   Container
    -   这就像一个包含视频文件、音频和元数据的篮子。您可以通过文件扩展名（如 .avi、.mov 或 .mp4）判断容器格式
-   Codecs
    -   这些压缩和解压算法旨在减小视频尺寸，同时保持视频质量。最常用的视频编解码器是 H.264、VP9 和 HEVC

##### Directed acyclic graph (DAG) model

对视频进行转码需要耗费大量计算成本和时间。此外，不同的内容创建者可能有不同的视频处理要求。例如，有些内容创建者要求在视频顶部添加水印，有些内容创建者要求自己提供缩略图，有些内容创建者要求上传高清视频，而有些内容创建者则不要求这样做。

为了支持不同的视频处理流水线并保持高度并行性，必须增加一定程度的抽象性，让客户端程序员定义要执行的任务。例如，Facebook 的流媒体视频引擎使用了有向无环图（DAG）编程模型，该模型分阶段定义任务，因此任务可按顺序或并行执行。

![](https://inasa.dev/image/systemdesign/14/6.png)

将原始视频分割成视频、音频和元数据。以下是可应用于视频文件的任务

-   Inspection
    -   确保视频质量上乘，没有畸形
-   Video encodings
    -   视频转换支持不同的分辨率、编解码器和比特率等
    -   ![](https://inasa.dev/image/systemdesign/14/7.png)
-   Thumbnail
    -   缩略图可以由用户上传，也可以由系统自动生成
-   Watermark
    -   视频顶部的图像覆盖层包含视频的识别信息

##### Video transcoding architecture

![](https://inasa.dev/image/systemdesign/14/8.png)

该架构有六个主要组件：预处理器、DAG 调度器、资源管理器、任务工作者、临时存储和作为输出的编码视频。

##### Preprocessor

![](https://inasa.dev/image/systemdesign/14/9.png)

预处理器有 4 项职责

-   Video splitting
    -   视频流被分割或进一步分割成更小的图片组（GOP）排列。GOP 是按特定顺序排列的帧组/块。每个片段都是一个可独立播放的单元，长度通常为几秒钟
-   某些老式移动设备或浏览器可能不支持视频分割。预处理器通过视频的 GOP 对齐方式
-   DAG generation
    -    处理器根据客户程序员编写的配置文件生成 DAG
    -   ![](https://inasa.dev/image/systemdesign/14/10.png)
    -   ![](https://inasa.dev/image/systemdesign/14/11.png)

-   Cache data
    -   预处理器是分段视频的缓存。为了提高可靠性，预处理器将 GOP 和元数据存储在临时存储器中。如果视频编码失败，系统可使用持久化数据进行重试操作

##### DAG scheduler

![](https://inasa.dev/image/systemdesign/14/12.png)

DAG 调度器将 DAG 图分割成阶段任务，并将其放入资源管理器的任务队列中。

![](https://inasa.dev/image/systemdesign/14/13.png)

原始视频分为三个阶段

-   第 1 阶段：视频、音频 和元数据
-   在第 2 阶段，视频文件被进一步分割成两个任务：视频编码和缩略图。音频文件需要进行音频编码，这是第二阶段任务的一部分

##### Resource manager

![](https://inasa.dev/image/systemdesign/14/14.png)

资源管理器负责管理资源分配的效率。它包含 3 个队列和一个任务调度器 

-   Task queue
    -   这是一个包含待执行任务的优先级队列
-   Worker queue
    -   它是一个优先级队列，包含工人利用率信息
-   Running queue
    -   它包含当前正在运行的任务和运行任务的工作者的信息
-   Task scheduler
    -   它选择最佳任务/工作者，并指示所选任务工作者执行任务

![](https://inasa.dev/image/systemdesign/14/15.png)

资源管理器的工作原理如下

-   任务调度程序从任务队列中获取优先级最高的任务
-   任务调度程序从工人队列中获取运行任务的最佳任务工人
-   任务调度程序指示所选任务工作者运行任务
-   任务调度程序会绑定任务/工作信息，并将其放入运行队列中
-   任务完成后，任务调度程序会将任务从运行队列中移除

##### Task workers

![](https://inasa.dev/image/systemdesign/14/16.png)

![](https://inasa.dev/image/systemdesign/14/17.png)

##### Temporary storage

![](https://inasa.dev/image/systemdesign/14/18.png)

这里使用多种存储系统。存储系统的选择取决于数据类型、数据大小、访问频率、数据寿命等因素。例如，元数据被工作人员频繁访问，而且数据量通常较小。因此，将元数据缓存在内存中是个好主意。对于视频或音频数据，我们将其放在 blob 存储中。一旦相应的视频处理完成，就会释放临时存储中的数据。

##### Encoded video

![](https://inasa.dev/image/systemdesign/14/19.png)

编码视频是编码流水线的最终输出结果

##### System optimizations

###### Speed optimization: parallelize video uploading

整体上传视频的效率很低。可以通过 GOP 对齐将视频分割成小块 

![](https://inasa.dev/image/systemdesign/14/20.png)

这样就能在上一次上传失败时快速恢复上传。客户端可执行按 GOP 分割视频文件的工作，以提高上传速度

![](https://inasa.dev/image/systemdesign/14/21.png)

###### Speed optimization: place upload centers close to users

提高上传速度的另一种方法是在全球建立多个上传中心。美国人可以将视频上传到北美上传中心，中国人可以将视频上传到亚洲上传中心。为此，我们使用 CDN 作为上传中心。

###### Speed optimization: parallelism everywhere

实现低延迟需要付出巨大努力。另一种优化方法是建立松耦合系统，实现高度并行性。

设计需要一些修改才能实现高度并行性

放大视频从原始存储器传输到 CDN 的流程

![](https://inasa.dev/image/systemdesign/14/23.png)

为了使系统更加松散耦合，引入了消息队列

-   在引入消息队列之前，编码模块必须等待下载模块的输出
-   引入消息队列后，编码模块不再需要等待下载模块的输出。如果消息队列中有事件，编码模块可以并行执行这些工作

![](https://inasa.dev/image/systemdesign/14/24.png)

###### Safety optimization: pre-signed upload URL

安全是任何产品最重要的方面之一。为确保只有授权用户才能将视频上传到正确的位置，引入预签名 URL

![](https://inasa.dev/image/systemdesign/14/25.png)

-   客户端向 API server发出 HTTP 请求，以获取预签名 URL，从而获得 URL 中标识的对象的访问权限
-   API Server使用预先签名的 URL 进行响应
-   客户端收到响应后，就会使用预先签名的 URL 上传视频

###### Safety optimization: protect your videos

许多内容制作者不愿意在网上发布视频，因为他们担心自己的原创视频会被盗用。为了保护受版权保护的视频，我们可以采用以下三种安全方案之一：

-   数字版权管理系统（DRM）： 三大 DRM 系统是 Apple FairPlay、Google Widevine 和 Microsoft PlayReady
-   AES 加密： 您可以加密视频并配置授权策略。加密视频将在播放时解密。这可确保只有授权用户才能观看加密视频
-   视觉水印： 这是在视频顶部叠加的图像，包含视频的识别信息。可以是公司徽标或公司名称

###### Cost-saving optimization

以前的研究表明，YouTube 视频流遵循长尾分布。这意味着少数热门视频的访问频率很高，但其他许多视频的观看者很少或根本没有观看者。

-   仅从 CDN 提供最受欢迎的视频，并从大容量存储视频服务器提供其他视频 
-   对于不太受欢迎的内容，可能不需要存储很多编码视频版本。短视频可按需编码
-   有些视频只在某些地区流行。没有必要将这些视频传播到其他地区
-   像 Netflix 一样建立自己的 CDN，并与互联网服务提供商（ISP）合作。建立自己的 CDN 是一项巨大的工程；不过，这对大型流媒体公司来说是有意义的。互联网服务提供商可以是 Comcast、AT&T、Verizon 或其他互联网提供商。互联网服务提供商遍布世界各地，离用户很近。通过与互联网服务提供商合作，可以改善观看体验并降低带宽费用。

##### Error handling

对于大型系统来说，系统错误是不可避免的。要建立一个高度容错的系统 系统，必须从容应对并快速恢复错误。有两种类型的错误存在

-   Recoverable error
    -   对于视频段转码失败等可恢复的错误，一般的做法是重试几次操作。如果任务继续失败，系统认为无法恢复，就会向客户端返回一个正确的错误代码
-   Non-recoverable error
    -   对于畸形视频格式等不可恢复的错误，系统会停止与视频相关的运行任务，并向客户端返回正确的错误代码

每个系统组件的典型错误都包含在以下操作手册中

-   Upload error: 重试几次
-   Split video error: 如果旧版本的客户端无法按 GOP 对齐方式分割视频，则会将整个视频传送到服务器。分割视频的工作在服务器端完成
-   Transcoding error: 重试
-   Preprocessor error: 重新生成 DAG 图
-   DAG scheduler error: 重新安排任务
-   Resource manager queue down: 使用复本
-   Task worker down: 在新的工作者上重试任务
-   API server down: API 服务器是无状态的，因此请求会被定向到不同的 API 服务器
-   Metadata cache server down: 数据会被多次复制。如果一个节点宕机，仍然可以访问其他节点获取数据。可以启动一个新的高速缓存服务器来替代瘫痪的服务器
-   Metadata DB server down
    -   Master is down
        -   如果主服务器宕机，则提升一个从服务器作为新的主服务器
    -   Slave is down
        -   如果一个从属服务器宕机，可以使用另一个从属服务器进行读取，并调用另一个数据库服来替代宕机的服务器

#### Step 4 - Wrap up

几点补充

-   Scale the API tier
-   Scale the database
-   Live streaming: 和非实时流媒体有一些相似之处：都需要上传、编码和 流。显著的不同之处在于
    -   实时流媒体对延迟要求较高，因此可能需要不同的流媒体协议
    -   实时流媒体对并行性的要求较低，因为小块数据已得到实时处理
    -   实时流媒体需要不同的错误处理方式。任何耗时过长的错误处理都是不可接受的
-   Video takedowns
    -   侵犯版权、色情或其他非法行为的视频将被删除。有些视频会在上传过程中被系统发现，有些则可能通过用户标记发现