
# 以太坊中的状态树

目标：账户地址到账户状态的映射。

address -> state
1. 账户地址address：
    - 地址是一个160位的二进制数，换成16进制就是40位（0x8f8b46E9183f5D8a89C9C7dC878eda5Ca2baFe6C）
    - 公钥取hash，然后截取了一部分。
2. 状态就是外部账户和合约账户的状态。
3. 状态包括了余额、 交易次数nonce，合约账户还包含了code和storage。

设计什么数据结构来实现`address -> state`的映射？

最简单的是hashtable，不考虑hash碰撞。

但是如果要提供merkle proof，怎么办？

无法提供merkle proof。

直接构成merkle tree的问题是问题在于：
1. merkle tree没有提供一个高效的查找或者更新的方法。
2. 如果不规定账户在叶节点的出现顺序，那么构建的merkle tree就是不一样的。
    - 比特币中也是不排序的，为什么没有这个问题？因为比特币中，发布区块的节点确定后再进行同步就行。
    - 以太坊中，账户状态是维护在本地的，如果广播出去做同步代价太大，而且一个区块的交易也只会改变很少的账户。
3. 那如果使用sorted merkle tree呢？
    - 新增账户怎么办？插入的代价太大。


## PT

trie tree，字典树

特点：
1. trie的查找速度取决于string的长度，以太坊的地址是固定的40位。
2. 哈希表可能出现碰撞，但trie不会出现碰撞。
3. 输入列表的元素顺序不影响最终的构造
4. 每次发布一个区块，系统中绝大多数账户不变，更新trie时不需要更改整个trie，意味着更新的局部性非常好。

由于存储开销大，引入了`Patricia trie`，相当于路径压缩后的`trie`。


优点：树的高度减小了、稳定了，访问内存的次数减少了，提升了查找效率和存储效率。

正是因为地址这个稀疏的特性，非常适合`Patricia trie`。

## MPT
MPT: Merkle Patricia trie
相当于`Patricia trie`，PMT就是哈希指针来替代了节点的值。

以太坊选用了MPT。