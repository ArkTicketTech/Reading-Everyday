
学习一下hackquest上的web3内容。

# blockchain data structures。

1. 区块链是如何组成的
2. 一个区块的数据结构
3. 一个交易的数据结构



`Ethereuum blockchain`可以由`blockchain, block, transaction`组成。

`transaction data, receipt data, state data, account data` ``都分别存储在四个独立的merkle tree。

# blockchain


由block组成的一条链。

# block


## block header

block header包括了

1. parentHash：上一个block的hash
2. timestamp：创建时间
3. nonce：POW挖矿中使用
4. difficulty：挖矿难度
5. coinBase：用来确认矿工地址

block header还记录了三个非常重要的root hash values，这里的root是指的merkle tree的root

1. stateRoot：block state tree（记录了所有账户的状态信息）的root hash
2. receiptRoot：receipt data（记录了交易执行的结果）的root hash
3. transactionRoot：transaction data（记录了所有的交易信息）的root hash



## block body

存储的交易Transaction集合列表。



# Transaction

Transaction 数据包括了：

1. nonce: 发送者账户的交易计数器，统计该账户在该区块链上进行的交易总数
2. gasPrice
3. gasLimit
4. to：接受者账户地址
5. value：转移eth数量
6. data：智能合约的字节码
7. v,r,s：发送者用私钥生成的交易签名

# Transaction Receipt

一旦交易完成，就会产生交易收据。交易收据记录了交易执行的基本信息，是交易被纳入区块链的重要证据。


交易收据包括了：

1. transactionHash：交易的hash
2. transactionIndex：该交易在block中的位置索引
3. blockHash：block hash
4. blockNumber： block number
5. from：发起交易的地址
6. to：目标地址
7. cumulativeGasUsed：当前block的累积消耗的gas
8. gasUsed：该交易消耗的gas
9. contractAddress：创建合约的地址，没有则为null
10. logs：event
11. logsBloom：交易日志的布隆过滤器
12. status：交易执行状态码，成功还是失败。



# Merkle Patricia Tree

merkle tree的一种特殊实现，简称MPT。

交易数据都存储在MPT中。

## Merkle tree

also known as a hash tree。

叶子结点是数据块的hash值，非叶子结点是子结点的hash值的和的hash值。

Hash A = Hash(Data A);

Hash B = Hash(Data B);

Hash E = Hash(Hash A + Hash B);


## Patricia Trie

压缩前缀树。



## Merkle Patricia Tree

结合了merkle tree和patricia tree的优点，可以快速验证数据的完整性并快速检索其信息。每个区块都包含一个Merkle Patricia Tree ，用于存储`transaction data, receipt data, state date, account data`。


