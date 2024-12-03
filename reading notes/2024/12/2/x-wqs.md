mac签名算法将秘钥和消息作为输入，生成一个非常短的标签，标签大概是90位或100位。消息长度是千兆字节，但标签长度很短。然后，将标签加入到信息message中，组合发给bob。收到消息和标签后，bob运行MAC验证算法：
verify tag:v（k,m,tag）=> yes or no 取决于消息是否被篡改。
MAC由两种算法构成，signing 和 verifying。defined over (K,M,T) where K is the key space, M is the message space, and T is the tag space.
Signing: S:K×M->T
Verifying: V:K×M×T-> {true,false}
使用同一秘钥签署和验证消息时，verify会返回true。


