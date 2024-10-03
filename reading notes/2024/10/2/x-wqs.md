forking attack，回滚交易。


每个区块包含的交易组织成Merkle tree的形式，由data blocks和hash pointers组成。下面的数据块都是交易。每个区块分为块头和块身。块头只有根哈希值，没有交易具体内容。block body 有交易列表。merkle tree的一个用途提供merkle proof。比特币中的节点分为两类，全节点(header和body都有)和轻节点(只保存header)。
