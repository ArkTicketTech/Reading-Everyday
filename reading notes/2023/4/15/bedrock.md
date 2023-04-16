架构

- Query Engine 查询引擎
  - 支持向量数据查询
  - 为现代CPU优化，减少 cache miss，利用 SIMD 指令
- GPU Engine
  - 协助 Query Engine 大规模并行
  - 支持多 GPU
- Storage Engine
  - LSM Tree based
  - 有 buffer pool
