区块链技术与应用-P2

# 哈希

- cryptographic hash function：两个性质
  1. collision resistance: that is, two inputs a and b where a != b, but Hash(a) = Hash(b).
     1. pigeonhole principle：输入空间远远大于输出空间，必然会发生碰撞
     2. 很难同时找到a和b，让他们的hash值相同。
  2. hiding：哈希值不可逆，很难从Hash(a)推断出a是什么。蛮力可以求解，但是在输入空间非常大且足够均匀，几乎等于不可求


通过cryptographic hash function，可以实现digital commitment，也叫做digital equivalent of a sealed envelope.

1. 现实中的一个问题：预测公开会影响预测结果，但预测不公开 如何知道预测没有被篡改？
2. 解决方法：如果提前公布的是hash值，那么结果发生后公开内容然后对比hash值就能知道是否被篡改过。
3. 这个方法利用了hiding的特性，但需要注意的是如果输入空间不够均匀效果也会打折扣，所以一般是再拼接一个随机值，使得输入更均匀。H(x || nonce)。



除了这两只性质外，比特币中还利用了puzzle friendly的性质，哈希值事先不可预测。puzzle friendly也是挖坑的工作量证明的基本特性，没有捷径，只有不断的暴力才能找到解。

difficult to solve, easy to verify


比特币中用的hash函数是SHA-256。

# 签名

1. 对称加密体系：共一个私钥加密解密，私钥保密。
2. 非对称加密体系：私钥加密，公钥解密；私钥保密，公钥公开。



比特币账户就相当于创造一个公钥和私钥。


签名用私钥，验证签名用公钥。

能够暴力的生成 私钥和公钥 对已存在的公钥私钥进行攻击，如果保证了随机源是优异的，目前的算力是不可能的。
