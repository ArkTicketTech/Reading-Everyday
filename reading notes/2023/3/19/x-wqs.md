URI的内容是一个存放网页数据的文件名或者是一个CGI程序的文件名。

访问Web服务器，获取网页数据使用GET方法。首先，在请求消息中写上GET方法，然后在URI中写上存放网页数据的文件名。当Web服务器收到消息后，会打开文件并读取出数据，然后将读出的数据存放到响应消息中，并返回给客户端。客户端浏览器会收到这些数据并显示在屏幕上。在表单中填写数据并将其发送给Web服务器时就会使用post方法。在请求消息中，除了方法和URI之外，还要加上传递给应用程序和脚本的数据。当服务器收到消息后，Web服务器会将请求消息中的数据发送给URI指定的应用程序。服务器从应用程序接收输出的结果，会将它存放到响应消息中并返回给客户端。

useragent 客户端软件的名称和版本号等相关信息。


发送者发出的消息首先经过子网中的集线器，转发到距离发送者最近的路由器上，路由器会根据消息目的地判断下一个路由器的位置，消息再次经过子网内的集线器被转发到下一个路由器，过程不断重复，最终消息传送到了目的地。

子网掩码是一串与IP地址长度相同的32比特数字，其左边一半都是1，右边一半都是0。其中，子网掩码为1的部分表示网络号，子网掩码为0的部分表示主机号。主机号部分比特全部为0，代表整个子网。主机号部分比特全部为1，表示对整个子网进行广播。
