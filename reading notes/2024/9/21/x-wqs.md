比特币称为加密货币，但是加密货币不加密，区块链上所有内容都是公开的，包括账户地址，转账金额。主要用到密码学中的哈希和签名。
cryptographic hash function
collision resistance
哈希冲突不可避免，因为输入远远大于输出
上传文件时先算哈希值，哈希值存在本地。下载之后，再算哈希值。跟原来存的哈希值比较下，如果是一样的，说明上传的文件没有被篡改。谁也没有能找到人为制造哈希碰撞的方法。
