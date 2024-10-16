
## MPT
MPT: Merkle Patricia trie
相对于`Patricia trie`，PMT就是把节点的普通指针换成了哈希指针。

根哈希值也是写在区块头上，保证了每个节点的本地的账户状态的merkle tree都是一样的，维护了一致性。

根hash值能够提供merkle proof。
1. 证明账户余额：merkle tree上的对应链条发送给轻节点，轻节点就可以进行验证。
2. 证明某个发生了交易的账户是否存在
3. 防止篡改

## Modified MPT
以太坊选用的是Modified MPT。

1. Extension Node：出现路径压缩的节点
2. branch node: 分支节点，表明从这里开始分叉。
3. leaf node：叶子结点
4. 1nibble = 4bits
5. 指针指向的是一个哈希值

modified的地方在于 有extension Node和branch node的区分。

根哈希值是要写在block header中的。

## 状态改变
每次发布一个新的区块的时，状态树中有一些节点的值会发生变化，这些改变不是在原地改的，而是新建一些分支，原来的状态保留下来。

合约账户也是用MPT存储的，合约账户里面还有一个小的MPT。

为什么要保留修改前的状态。

因为软分叉是随时都会有的，当最长链胜出的时候，为了使得节点能够同步到最长链上，就需要回滚到分叉点再进行连接最长链，所以保留历史状态是必要的。

## 以太坊代码中的数据结构
### block header
1. ParentHash
2. UncleHash
3. Coinbase
4. Root: 状态树的root hash
5. TxHash：交易树的root hash
6. ReceiptHash: 收据树的root hash




### 区块结构
1. header: 指向blocker header的指针
2. uncles：指向叔父区块的指针
3. transactions: 交易列表



### extblock
区块在网上真正发布时的信息：


## RLP

状态树中保存的是<key, value> pair，key就是地址，而value是通过RLP后保存的。

RLP: Recursive Length Prefix，一种进行序列化的方法.

常用的库：Protocol buffer（protobuf)。

只支持一种类型：nested array of bytes.








