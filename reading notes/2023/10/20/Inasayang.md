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