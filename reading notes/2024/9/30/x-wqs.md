SHA-256
secure hash algorithm
三个性质都满足，collision resistance hiding puzzle friendly

账户
每个人可以自己决定开户，不需要任何批准。开户过程就是创立公钥私钥对。公钥相当于银行账号私钥相当于密码。签名用私钥，验证签名用公钥。


新发布交易是否合法，需要查一下UTXO。币在Utxo集合里才合法。全节点在内存中维护Utxo数据结构，以便快速检测double spending。
