## WebSocket

WebSocket 是一种在单个 TCP 连接上进行全双工通信的协议，它使得客户端和服务器之间可以进行实时、双向的数据传输。相比传统的 HTTP 请求-响应模式，WebSocket 允许在一个持久连接上进行双向通信，从而降低了通信的延迟。

WebSocket 的主要特点包括：

1. **全双工通信：** WebSocket 支持双向通信，客户端和服务器可以独立地向对方发送数据。
2. **持久连接：** WebSocket 建立连接后可以保持打开状态，而不需要在每次通信时都重新建立连接。这有助于减少通信的延迟和降低服务器和客户端之间的负担。
3. **轻量级：** WebSocket 协议相对较轻量，头部信息较小，减少了通信的开销。
4. **实时性：** WebSocket 使得实时性的应用更容易实现，例如聊天应用、实时数据更新等。

### WebSocket 握手过程：

1. **客户端发送 WebSocket 握手请求：**
   - 客户端发起 HTTP 请求，其中包含一些特殊的头部，表明它希望升级连接为 WebSocket。
2. **服务器响应 WebSocket 握手：**
   - 服务器收到请求后，如果同意升级为 WebSocket，将响应的状态码设置为 101 Switching Protocols，并附上一些必要的头部信息。
3. **建立 WebSocket 连接：**
   - 客户端和服务器建立 WebSocket 连接，此后双方可以通过这个连接进行实时的双向通信。

### 示例：

**WebSocket 握手请求示例：**

```
httpCopy code
GET /chat HTTP/1.1
Host: example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==
Sec-WebSocket-Protocol: chat, superchat
Sec-WebSocket-Version: 13
Origin: http://example.com
```

**WebSocket 握手响应示例：**

```
httpCopy code
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=
Sec-WebSocket-Protocol: chat
```

WebSocket 协议的使用场景包括实时聊天、在线游戏、股票市场的实时更新等需要低延迟和高实时性的应用。在 Web 开发中，通常可以使用 JavaScript 的 WebSocket API 进行 WebSocket 连接的建立和数据传输。