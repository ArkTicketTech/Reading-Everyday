# benchmarked method
HNSW Post-filtering - 选出 K/s 个近邻 随后过滤
Pre-filtering - 先过滤 然后暴力选出最近的节点

Filtered-DiskANN - FilteredVamana and StitchedVamana

NHQ

Milvus - IVF-Flat, IVF-SQ8, HNSW, and IVF-PQ

Oracle Partition Index

> We will then present an evaluation on the HCPS datasets. On these datasets, the FilteredDiskANN and NHQ algorithms fail because they assume are unable to handle the high cardinality query predicate sets and non-equality predicate oper- ators.


LCP 性能比较 - even compared to NHQ and FilteredDiskANN, which specialize for LCPS datasets, ACORN- 𝛾 consistently achieves 2-10× higher QPS at fixed recall values.

（不过 filteredDiskAnn 是为了容量达到内存极限设计的🤔

> This is because while ACORN-1 and ACORN-𝛾 almost exclusively traverse over nodes that pass the query predicates, the post-filtering algorithm is less discriminating and wastes distance computations on nodes failing the query predicate.



