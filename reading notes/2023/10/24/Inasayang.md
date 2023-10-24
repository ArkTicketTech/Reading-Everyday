## CHAPTER 15: DESIGN GOOGLE DRIVE

### Step 1 - Understand the problem and establish design scope

-   Candidate: 最重要的功能是什么？
-   Interviewer: 上传和下载文件、文件同步和通知
-   Candidate: 这是一个移动应用程序，一个网络应用程序，还是两者都有？
-   Interviewer: 两者都有
-   Candidate: 支持哪些文件格式？
-   Interviewer: 任何文件类型
-   Candidate: 文件需要加密吗？
-   Interviewer: 是的，存储中的文件必须加密
-   Candidate: 文件大小有限制吗？
-   Interviewer: 是，文件必须为 10 GB 或更小
-   Candidate: 产品有多少用户？
-   Interviewer: 1 000 万 DAU

features

-   Add files
-   Download files
-   Sync files across multiple devices
-   See file revisions
-   Share files with your friends, family, and coworkers
-   Send a notification when a file is edited, deleted, or shared with you

Features not discussed

-   Google doc editing and collaboration

non-functional requirements

-   Reliability
-   Fast sync speed
-   Bandwidth usage
-   Scalability
-   High availability

#### Back of the envelope estimation

-   Assume the application has 50 million signed up users and 10 million DAU
-   Users get 10 GB free space
-   Assume users upload 2 files per day. The average file size is 500 KB
-   1:1 read to write ratio
-   Total space allocated: 50 million * 10 GB = 500 Petabyte
-   QPS for upload API: 10 million * 2 uploads / 24 hours / 3600 seconds = ~ 240
-   Peak QPS = QPS * 2 = 480

### Step 2 - Propose high-level design and get buy-in

start with a single server setup

-   A web server to upload and download files
-   A database to keep track of metadata like user data, login info, files info, etc
-   A storage system to store files. Allocate 1TB of storage space to store files

Spending a few hours setting up an Apache web server, a MySql database, and a directory
called drive/ as the root directory to store uploaded files

Under drive/ directory, there is a list of directories, known as namespaces

Each namespace contains all the uploaded files for that user

The filename on the server is kept the same as the original file name

Each file or folder can be uniquely identified by joining the namespace and the relative path

#### APIs

3 APIs: upload a file, download a file, and get file revisions

##### Upload a file to Google Drive

-   Simple upload: 当文件较小时，使用这种上传类型
-   Resumable upload: 当文件大小较大且网络中断几率较高时，请使用这种上传类型

https://api.example.com/files/upload?uploadType=resumable
Params:
• uploadType=resumable
• data: Local file to be uploaded.



A resumable upload is achieved by the following 3 steps

-   发送初始请求以检索可恢复的 URL
-   上传数据并监控上传状态
-   如果上传受到干扰，恢复上传



##### Download a file from Google Drive

Example API: https://api.example.com/files/download
Params:
• path: download file path.
Example params:
{
	"path": "/recipes/soup/best_soup.txt"
}



##### Get file revisions

Example API: https://api.example.com/files/list_revisions
Params:
• path: The path to the file you want to get the revision history.
• limit: The maximum number of revisions to return.
Example params:
{
	"path": "/recipes/soup/best_soup.txt",
	"limit": 20
}

#### Move away from single server

 an example of sharding based on user_id

![](https://inasa.dev/image/systemdesign/15/1.png)

使用S3

![](https://inasa.dev/image/systemdesign/15/2.png)

![](https://inasa.dev/image/systemdesign/15/3.png)

#### Sync conflicts

如何解决冲突呢？策略是：先处理的版本胜出，而后处理的版本会收到冲突。

![](https://inasa.dev/image/systemdesign/15/4.png)

用户 1 和用户 2 试图同时更新同一个文件，但用户 1 的文件会先被我们的系统处理

用户 1 的更新操作通过了，但用户 2 出现了同步冲突

系统会同时显示同一文件的两个副本：用户 2 的本地副本和来自服务器的最新版本。用户 2 可以选择合并两个文件，或者用另一个版本覆盖其中一个版本

#### High-level design

![](https://inasa.dev/image/systemdesign/15/5.png)

### Step 3 - Design deep dive

#### Block servers

对于定期更新的大文件，每次更新时发送整个文件会消耗大量带宽。为了最大限度地减少网络传输流量，提出了两种优化方案：

-   Delta sync
    -   修改文件时，只同步修改的区块，而不是使用同步算法同步整个文件 
-   Compression
    -   对数据块进行压缩可以大大减小数据大小
    -   根据文件类型使用压缩算法对数据块进行压缩。例如，gzip 和 bzip2 用于压缩文本文件。压缩图像和视频则需要不同的压缩算法

Block servers通过将文件分割成区块、压缩每个区块并对其进行加密，来处理客户端传过来的文件。只传输修改过的块，而不是将整个文件上传到存储系统。

![](https://inasa.dev/image/systemdesign/15/6.png)



![](https://inasa.dev/image/systemdesign/15/7.png)

delta  sync，即只有修改过的区块才会传输到云存储。突出显示的区块 "区块 2 "和 "区块 5 "代表已更改的区块

#### High consistency requirement

系统默认要求很强的一致性。不同的客户端在同一时间显示不同的文件是不可接受的。系统需要为元数据缓存和数据库层提供强一致性。

内存缓存默认采用最终一致性模型，这意味着不同的副本可能拥有不同的数据。要实现强一致性，必须确保以下几点：

-   缓存副本中的数据与主数据一致
-   在写入数据库时使缓存失效，以确保缓存和数据库保持相同的值

在关系数据库中实现强一致性很容易，因为它能保持 ACID（原子性、一致性、隔离性、持久性）属性 

然而，NoSQL 数据库默认不支持 ACID 属性。ACID 属性必须以编程方式纳入同步逻辑

#### Metadata database

![](https://inasa.dev/image/systemdesign/15/8.png)

-   User：用户表包含用户的基本信息，如用户名、电子邮件、个人照片等
-   Device：设备表存储设备信息。Push_id 用于发送和接收移动推送通知。请注意，一个用户可以拥有多个设备
-   Namespace：命名空间是用户的根目录
-   File：文件表存储与最新文件相关的所有内容
-   File_version：它存储文件的历史版本。现有行为只读，以保持文件修订历史的完整性
-   Block：它存储与文件块相关的所有内容。按照正确的顺序连接所有文件块，就能重建任何版本的文件

#### Upload flow

![](https://inasa.dev/image/systemdesign/15/9.png)

并行发送两个请求：添加文件元数据和将文件上传到云存储。两个请求都来自客户端 1

-   Add file metadata
    -   客户端 1 发送添加新文件元数据的请求
    -   在metadata DB 中存储新文件元数据，并将文件上传状态更改为 "pending"
    -   通知通知服务正在添加新文件
    -   通知服务会通知相关客户端（客户端 2）正在上传文件
-   Upload files to cloud storage
    -   客户端 1 将文件内容上传到block server
    -   block server将文件分成块，对块进行压缩和加密，然后上传到云存储空间
    -   文件上传后，云存储会触发上传完成回调。请求被发送到 API 服务器
    -   metadata DB 中的文件状态更改为 "uploaded"
    -   在文件状态更改为 "uploaded "时通知通知服务
    -   通知服务会通知相关客户端（客户端 2）文件已完全上传

编辑文件时的流程与此类似

#### Download flow

在其他地方添加或编辑文件时会触发下载流。客户端如何知道文件是否被其他客户端添加或编辑？客户端可以通过两种方式知道：

-   如果 A 客户端在线，而另一个客户端更改了文件，通知服务会告知 A 客户端某处发生了更改，因此它需要提取最新数据
-   如果客户端 A 处于离线状态，而另一个客户端正在更改文件，数据将被保存到缓存中。当离线客户端再次上线时，它就会提取最新的更改

一旦客户端知道文件发生了更改，它首先会通过 API 服务器请求元数据，然后 下载块来构建文件。

![](https://inasa.dev/image/systemdesign/15/10.png)

#### Notification service

为了保持文件的一致性，本地执行的任何文件变更都需要通知其他客户端，以减少冲突。通知服务就是为实现这一目的而构建的。在高层，通知服务允许在事件发生时向客户端传输数据。以下是几个选项：

-   Long polling
    -   Dropbox uses long polling
-   WebSocket



尽管两种方案都很有效，但还是选择了Long polling，原因有以下两个

-   通知服务的通信不是双向的。服务器会向客户端发送文件更改信息，但反之亦然
-   WebSocket 适用于实时双向通信，如聊天应用程序。对于 Google Drive 而言，通知的发送频率较低，不会出现数据突发的情况

通过长轮询，每个客户端都会与通知服务建立一个长轮询连接

如果检测到文件有变化，客户端将关闭长轮询连接

关闭连接意味着客户端必须连接到元数据服务器以下载最新更改

在收到响应或连接超时后，客户端会立即发送新请求以保持连接打开

#### Save storage space

为支持文件版本历史并确保可靠性，同一文件的多个版本会存储在多个数据中心。如果频繁备份所有文件版本，存储空间很快就会被占满。提出三种降低存储成本的技术：

-   De-duplicate data blocks
    -   在账户级别消除冗余块是节省空间的一个简单方法。如果两个区块的哈希值相同，那么它们就是相同的
-   Adopt an intelligent data backup strategy
    -   Set a limit
        -   可以设置存储版本的数量限制。如果达到限制，最旧的版本将被新版本取代
    -   Keep valuable versions only
        -   有些文件可能会经常被编辑。例如，如果对修改较多的文件保存每个编辑版本，可能意味着该文件在短时间内被保存超过 1000 次。为了避免不必要的拷贝，可以限制保存版本的数量。更看重最近的版本。实验有助于找出保存版本的最佳数量
-   Moving infrequently used data to cold storage
    -   冷数据是指数月或数年内没有活动的数据
    -   亚马逊 S3 glacier 等冷存储比 S3 便宜得多

#### Failure Handling

-   Load balancer failure
-   Block server failure
-   Cloud storage failure
-   API server failure
-   Metadata cache failure
-   Metadata DB failure
-   Notification service failure
-   Offline backup queue failure

### Step 4 - Wrap up

例如，可以直接从客户端将文件上传到云存储，而不是通过block server。这种方法的优点是文件上传速度更快，因为文件只需传输一次到云存储。在之前的设计中，文件首先传输到block server，然后再传输到云存储。不过，这种新方法也有一些缺点：

-   首先，必须在不同平台（iOS、Android 和 Web）上执行相同的分块、压缩和加密逻辑。这很容易出错，而且需要大量的工程设计工作。在之前的设计中，所有这些逻辑都集中在一个地方实现：block server
-   其次，由于客户端很容易被黑客攻击或操纵，在客户端实施加密逻辑并不理想

系统的另一个有趣的演变是将在线/离线逻辑转移到一个单独的服务中。称之为 "在线服务"。通过将在线服务移出通知服务器，在线/离线功能可以很容易地被其他服务集成。