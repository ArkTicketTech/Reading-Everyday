1. vpn 路由是如何配置的？

在 server.conf 中可以填写 "push ip mask gateway" 规则； 客户端连接时会从服务端获取规则，并保存在本地的路由表中。

```
Internet:
Destination        Gateway            Flags               Netif Expire
0/1                10.8.0.1           UGScg               utun4
default            192.168.199.1      UGScg                 en0
default            link#21            UCSIg           bridge100      !
10.8/24            10.8.0.2           UGSc                utun4
10.8.0.1           10.8.0.2           UHr                 utun4
```

0/1 表示默认请求都走 utun4

2. 两层路由原理

先根据0层路由规则 访问0层ip地址命中 发往0层隧道
0层隧道实际上会发往 0 层 vpn server
这个 ip 可能会命中 1层 路由规则 发往 1层隧道
1层隧道对应的 ip 在路由规则中通常就是指向了 en0 等设备

发到第一层 vpn server 拆包之后会转发到 0 层 vpn server

