为什么只用块头信息吗？
轻节点只下载块头，就可以查验是否符合挖矿难度要求了。

重要特点：
读取dataset的这两个哈希值，在位置上是关联的，但生成的过程是独立的。这是构造大数据集的重要特点，每个元素独立生成，给轻节点的验证提供了方便。

区别：
轻节点没有保存大数据集，要从cache重新生成。轻节点是临时计算出用到的dataset元素，而矿工是直接访存，必须在内存中存着这个1G的dataset。
