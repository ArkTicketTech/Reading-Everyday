谷歌身份验证原理基于TOTP（Time-Based One-Time Password），TOTP是基于时间的一次性密码算法。
1. 一个共享密钥secret: 共享密钥在服务端建立，密钥内容由用户输入到身份验证器中
2. 一个签署方法HMAC-SHA1：hmac = SHA1(secret + SHA1(secret + input))
3. 输入input：一般是当前时间戳，或者是计数时间。

服务端和客户端用共享的密钥和当前时间戳来运算同一个方法，如果是同一个密钥，并且在时间范围内，那么最后得到的code就是一样的，所以2FA的身份验证要求你输入生成的code。

共享密钥需要妥善保管，即使当前设备丢失，也可以进行恢复。
