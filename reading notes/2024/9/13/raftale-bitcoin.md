## Transition-based ledger

比特币采用了基于交易的账本（Transition-based ledger）模式，并维护了一个UTXO(Unspent Transation Output) 的数据结构，来防止双花攻击，也就是说，交易中币的来源必须是在UTXO中。



矿工打包交易的动机：

1. block reward: 每隔21万个区块减半，大概4年一次。
2. transaction fee：`total outpus - total inputs`

### Block Header

1. version
2. Previous block header hash
3. merkle root hash
4. time: 区块产生时间
5. nBits: An encoded version of the target threshold this block's header hash must be less than or equal to.
6. nonce

### 挖矿

挖矿本身就是在尝试`nonce`，使得产生的区块哈希值小于等于目标阈值。







