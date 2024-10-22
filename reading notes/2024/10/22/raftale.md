
Socket API 最初是为Unix操作系统开发的，但现在几乎所有的操作系统和编程语言中都有变体存在。

三次握手和四次挥手都是在tcp层完成的，客户端的http层并没有参与。

三次握手的处理是Linux系统内核本身处理的，而不是nginx or apache这样的进程去处理，nginx or apache处理的是http请求。

建立了请求后什么时候会断开？

设置header中的connection: keep-alive可以保证一定时间不断开，时间的长短是由服务端决定的。例如nginx默认75s内没有新的请求就会断开。

HTTPS的SSL连接也是在tcp三次握手的时候完成的。
