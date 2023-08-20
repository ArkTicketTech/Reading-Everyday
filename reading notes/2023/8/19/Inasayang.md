### CHAPTER 5: DESIGN CONSISTENT HASHING

要实现横向扩展，就必须在服务器之间高效、均匀地分配请求/数据。一致散列是实现这一目标的常用技术

#### The rehashing problem

有 n 台缓存服务器，负载均衡的常用方法是使用下面的散列方法

`serverIndex = hash(key) % N, where N is the size of the server pool.`



![](https://inasa.dev/image/systemdesign/05/1.png)

![](https://inasa.dev/image/systemdesign/05/2.png)

在服务器池大小固定、数据分布均匀的情况下效果很好。

当增加新服务器或删除现有服务器时，问题就会出现。

![](https://inasa.dev/image/systemdesign/05/3.png)

![](https://inasa.dev/image/systemdesign/05/4.png)

导致缓存丢失风暴



Pp. 71-74