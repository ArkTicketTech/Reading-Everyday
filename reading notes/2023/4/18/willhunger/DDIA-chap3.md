### Column-Oriented Storage
#### 1. Column Compression
压缩数据节省磁盘吞吐量。

一种压缩方式：位图优化。如果列中的 distinct value 特别少，则可以考虑使用位图优化节省存储。如果 distinct value 分布稀疏，则可以使用 run-length encoded 进行优化。

bitmap index 对这类查询非常合适：
* `WHERE product_sk in (30, 68, 69)`
* `WHERE product_sk = 31 AND store = 3`

column families 概念：cassandra 和 HBase 特有的概念，来源于 BigTable 中的宽列，其将多个列组织称一行进行存储，且不使用列压缩。column families modle 大多数是 row-oriented 的。

##### memory bandwidth and vectorized processing
数仓通常需要读大量数据，瓶颈有：
* 内存带宽
* 主存和 CPU cache 之间的带宽
* 避免分支预测，以及 CPU 流水线中插入的 bubbles 导致的 stale
* 如何充分利用 SIMD

column oriented 存储布局的优势：
* 压缩。节省内存带宽
* 处理数据时的向量化处理（vectorized processing）能力。可以直接 a chunk of compressed column data 传送至 CPU L1 cache，使用一个 tight loop （无额外函数调用）进行处理。