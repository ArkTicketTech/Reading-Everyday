![image](https://github.com/user-attachments/assets/f3f43e69-8510-4696-b3c9-4de029666598)

以太坊用于存储执行层状态的主要数据结构是 Merkle Patricia Trie（发音为 “try”）。它之所以被命名为 Merkle Patricia Trie，是因为它结合了 Merkle 树的特性，并采用了 PATRICIA（Practical Algorithm To Retrieve Information Coded in Alphanumeric）算法的特点，同时它的设计旨在高效地检索构成以太坊状态的各个数据项。

以太坊的状态被存储在四个不同的修改版 Merkle Patricia Tries（MMPTs）中：

- 交易 Trie（Transaction Trie）
- 回执 Trie（Receipt Trie）
- 世界状态 Trie（World State Trie）
- 账户状态 Trie（Account State Trie）
