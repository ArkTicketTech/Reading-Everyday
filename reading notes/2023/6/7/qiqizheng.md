# \## 03-03-06 OPTIONS 和 TRACE方法

客户端发起一个请求时， 这个请求可能要穿过防火墙、代理、网关或其他一些应用程序。  每个中间节点都可能会修改原始的HTTP请求。 
TRACE方法允许客户端在最终将请求发送个服务器时，看看它变成了什么样子。

TRACE请求会在目的服务器端发起一个“环回”诊断。行程最后一站的服务器会弹回一条TRACE响应， 并在响应主体中携带它收到的原始请求报文。    这样客户端就可以查看原始报文唉请求/响应链上， 是否被毁坏或修改过。

我理解的TRACE方法， 就是客户端可以查看到请求报文， 是否在请求/响应链中，被修改毁坏。

OPTIONS方法请求Web服务器告知其支持的各种功能。  可以询问服务器通常支持哪些方法， 或者对某些特殊资源支持哪些方法。  

OPTIONS是请求服务器，并告知支持哪些功能。 

**出处**：[Gourley David,Totty Brian《HTTP权威指南》(2012) P](zotero://select/library/items/WK5NQJZ4)60
