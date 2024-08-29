笔记

比特币中的数据结构。
# 区块链

哈希指针：除了指向地址，也保存了结构体的hash值。
Block chain is a linked list using hash pointer.

对每个区块头进行SHA256加密哈希，可生成一个哈希值，
每个区块都可以通过其区块头的「父区块哈希值」字段引用前一区块。

与普通链表的区别是：

普通链表可以随意改变一个节点，但是区块链中改变任何一个区块，都必须改变区块后的所有区块来保证「父区块哈希值」的索引。
tamper-evident log


# merkle tree
Merle tree是一种哈希二叉树。
父节点的哈希值是子节点哈希值相加后哈希值。

好处：
记住了根hash值，就能检测出对树中任何部位的修改。


1. block header: including root hash
2. block body: 交易的链表


用途：提供merkle proof

比特币中的节点分为两类：
1. 全节点：包含block header和block body
2. 轻节点：只保存block header

轻节点如何证明某个交易在区块链中？
1. 找到交易区块
2. 向全节点请求包含该交易的hash值；
3. 最后自己做验证，根hash值与自己计算的hash值一样则没有问题。

证明某个交易在区块链中也叫做：Proof of membership/inclusion.
时间复杂度为O(logN).

如何证明merkle tree 没有包含某个交易？
1. 穷举树，时间复杂度为O(N)
2. 排序 sorted merkle tree，然后二分查找，比特币没有使用，它没有这种需求


哈希指针不用用于环形链表，容易出错。










