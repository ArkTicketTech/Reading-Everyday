ping 服务器A的IP，B却去查询默认网关的MAC地址，B根据子网掩码计算出来A属于不同子网，跨子网通信需要默认网关的转发。要和默认网管通信，就需要获得其MAC地址。MAC地址前三个字节代表厂商。
Capture-->Options，Limit each packet to, 80字节
协议过滤时，考虑到协议间的依赖关系。NFS共享挂载失败，portmap || mount来过滤
IP地址加port号是最常用的过滤方式，ip.addreq<IP地址> && tcp.porteq<端口号>
直接选择归纳兴趣的内容，prepare a filter --> selected， 在Filter框中自动生成过滤表达式
Ethereal，还原以太网的真相
Network File System NFS服务器提供/code，/document两个共享目录，当用户在这些本地目录读写文件时，实际是在NFS服务器上读写。
