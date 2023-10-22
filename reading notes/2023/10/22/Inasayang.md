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

s