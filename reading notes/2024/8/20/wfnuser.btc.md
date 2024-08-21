1. collision resistance \ collision free
   已知 x 没有什么好办法 - 找到一个 y 使得 H(y) = H(x) - 只有 Brute-Force

H(m) -> 可以认为是 message 的 digest -> 无法篡改内容而不被检测出来

$$x \neq y, \quad H(x) = H(y)$$

没有哪个哈希函数能在数学上证明 collision resistance
只能通过实践检验 - 有些哈希函数以前我们认为是 resistance 的 - 后来找到了认为制造哈希碰撞的方式了 - 比如 md5

2. hiding
   单向 不可逆的
   x -> H(x)

这两个性质可以实现 digital commitment 也叫 digital equivalent of a sealed envelope

实际操作中会增加一个随机的 nonce H(x || nonce)

btc 中还要求一个性质