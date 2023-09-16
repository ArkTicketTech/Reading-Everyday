### CHAPTER 12: DESIGN A CHAT SYSTEM

#### Step 1 - Understand the problem and establish design scope

设计哪种类型的聊天应用至关重要。市场上有 Facebook Messenger、微信和 WhatsApp 这样的一对一聊天应用，也有 Slack 这样专注于群聊的办公聊天应用，还有 Discord 这样专注于大型群组互动和低语音聊天延迟的游戏聊天应用。

-   Candidate: 什么类型的聊天app？1对1还是群组？
-   Interviewer: 都支持
-   Candidate: 手机app？web app？还是都有？
-   Interviewer: 都有
-   Candidate: 规模有多大？是初创应用还是大规模应用？
-   Interviewer: 应支持 5000 万日活跃用户（DAU）
-   Candidate: 群聊的群成员上限是多少？
-   Interviewer: 最大一百人
-   Candidate: 聊天应用程序有哪些重要功能？能否支持附件？
-   Interviewer: 一对一聊天、群聊、在线指标。系统仅支持文本信息。
-   Candidate:  有信息大小限制吗？
-   Interviewer: 文本长度应少于 100 000 个字符
-   Candidate: 是否需要端到端加密？
-   Interviewer: 暂时不需要，但如果时间允许，我们会讨论这个问题
-   Candidate: 聊天记录要保存多久？
-   Interviewer: 永久



-   一对一聊天，传输延迟低
-   小组聊天（最多 100 人）
-   在线存在
-   支持多种设备。同一账户可同时登录多个账户
-   推送通知



#### Step 2 - Propose high-level design and get buy-in

-   接收其他客户的信息
-   为每条信息找到合适的收件人，并将信息转发给收件人
-   如果收件人不在线，则在服务器上保留该收件人的信息，直到在线为止

![](https://inasa.dev/image/systemdesign/12/1.png)

客户端打算开始聊天时，会使用一个或多个网络协议连接聊天服务。对于聊天服务来说，网络协议的选择非常重要

大多数客户端/服务器应用程序的请求都是由客户端发起的。聊天应用程序的发送方也是如此。发送方通过聊天服务向接收方发送信息时，使用的是久经考验的 HTTP 协议，这是最常见的网络协议。在这种情况下，客户端会打开与聊天服务的 HTTP 连接并发送信息，通知聊天服务将信息发送给接收方。keep-alive 在这种情况下非常有效，因为 keep-alive 头允许客户端与聊天服务保持持久连接。它还能减少 TCP 握手的次数。HTTP 对发送方来说是个不错的选择，许多流行的聊天应用程序，如 Facebook 等许多流行的聊天应用程序最初都使用 HTTP 发送信息。

不过，接收方的情况要复杂一些。由于 HTTP 是由客户端发起的，因此从服务器发送信息并非易事。多年来，人们使用了许多技术来模拟服务器发起的连接：轮询、长轮询和 WebSocket。



##### Polling

![](https://inasa.dev/image/systemdesign/12/2.png)

轮询是客户端定期询问服务器是否有信息可用的一种技术。根据轮询频率的不同，轮询的成本可能很高。它可能会消耗宝贵的服务器资源来回答一个大部分时间都以 "否 "作为答案的问题。



##### Long polling

![](https://inasa.dev/image/systemdesign/12/3.png)

在长轮询中，客户端保持连接开放，直到有新信息或达到超时阈值。一旦客户端收到新信息，它就会立即向服务器发送另一个请求，重新启动进程。长轮询有几个缺点：

-   发送方和接收方可能无法连接到同一个聊天服务器。基于 HTTP 的服务器通常是无状态的。如果使用轮循进行负载平衡，接收消息的服务器可能无法与接收消息的客户端建立长时间轮循连接
-   服务器无法很好地判断客户端是否断开连接
-   效率低。如果用户不怎么聊天，长轮询在超时后仍会进行周期性连接



Pp. 178-182