以太坊中主要有两种类型的交易池：

1. **Legacy 池**：由 Geth 管理，这些池使用基于价格排序的堆或优先队列来组织交易。具体来说，交易使用两个堆来组织：一个优先考虑即将到来的区块的有效小费，另一个关注 gas 费用上限。在交易池饱和期间，会选择这两个堆中较大的一个来驱逐交易，从而优化池的效率和响应性。[紧急和浮动堆(urgent and floating heaps)](https://github.com/ethereum/go-ethereum/blob/064f37d6f67a012eea0bf8d410346fb1684004b4/core/txpool/legacypool/list.go#L525)

2. **Blob 池**：与传统池不同，blob 池维护一个采用不同机制剔除交易的优先堆。值得注意的是，blob 池的实现有详细的文档，可以在[这里](https://github.com/ethereum/go-ethereum/blob/064f37d6f67a012eea0bf8d410346fb1684004b4/core/txpool/blobpool/blobpool.go#L132)查看详细的注释说明。blob 池的一个关键特征是在其驱逐队列中使用对数函数。