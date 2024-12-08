## 通信过程
HTTPS首先要建立通信，开始传输信息的前提是证书要通过验证。
1. 客户端向服务端发送请求: 包括了SSL/TLS版本和一个随机数
2. 服务端向客户端返回：签名证书和一个随机数
3. 客户端解析证书，获取到了服务端的公钥
4. 客户端生成新的公私钥对，公钥叫做pre-master key，然后用服务端的公钥对pre-master key进行加密后发送给服务端，服务端用自己的私钥解密，获取到了pre-master key。
5. 此时客户端和服务端都有了：客户端的随机数 + 服务端的随机数 + 客户端生成的pre-master key，双方用约定的算法和这三个元素就可以生成一个密钥对(shared secret)，用于后续的对称加密。
6. 对称密钥进行明文加密传输，公钥加密，私钥解密。