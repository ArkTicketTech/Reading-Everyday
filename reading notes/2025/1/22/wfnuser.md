superfluid 原理
- super token wrap erc20
- super token 根据区块时间和 flowrate 计算实际的 token 数量
- distribution pool 创建后指向一些地址
- 地址对应的合约需要自己发起 connectPool 到对应的 pool 才可以感知到 super token 的变化
- 发起流的时候需要先 approve erc20 从当前账户转到合约账户 - 然后合约账户 approve erc20 - 把合约持有的 erc20 换成 super token

