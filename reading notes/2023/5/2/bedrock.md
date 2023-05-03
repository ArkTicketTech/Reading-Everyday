Volcano模型有几个特征：
- Extensible：Operator之间依赖于接口而非具体实现，很容易实现新的Operator替换现有的
- Parallel：每个Operator可以在不同的线程运行，next不再是函数调用而是进程间通信，即可实现Operator之间的流水线化并行
- Pipeline：每次调用next接口只需要返回一条数据，不需要把所有数据计算完成再返回，避免过多的数据物化的开销
- Iterator pull：虽然称之为火山，但数据并不是从Plan Tree的底部网上喷发的，而是从顶部把数据通过迭代器pull上去的
🔗 原文链接： https://zhuanlan.zhihu.com/p/60965109
