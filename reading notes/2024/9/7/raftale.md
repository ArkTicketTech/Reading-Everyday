- BTC系统挖矿，会不会有矿工偷窃答案？例如：某个矿工发现其他矿工发布了nonce，收到后验证该区块合法，将该nonce作为自己找到的nonce发布出去。

  - `Coinbase Transaction`是每个区块的第一个交易，作为对求解`nonce`的矿工的奖励。需要注意`nonce` 和`merkle root hash` 是都在`block header`中，验证`nonce`是通过`H(block header) <= target`，如果其他矿工验证通过想要偷答案，就需要修改交易中的收款地址，就会改变`merkle root hash`，从而导致原本的nonce作废。
