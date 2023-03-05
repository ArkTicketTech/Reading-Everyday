单机数据库上的 Join 是怎么做的。MySQL 支持的 Join 算法很有限：

- Nested-Loop Join (NL Join)
- Batched Key Access Join (BKA Join)
- Block Nested-Loop Join（版本 < 8.0.20）
- Hash Join (版本 >= 8.0.18)

如果 Join 两侧的任何一张表上 join key 列存在索引，那么 MySQL 通常会使用基于索引的 BKA Join 或 NL Join，我们实际使用中的绝大多数情形都对应这种方式。如果 Join 两侧都没有索引可以用，那么 MySQL 只能退而求其次选择 Block Nested-Loop Join 或 Hash Join（取决于 MySQL 版本）。我们今天主要关注 NL 和 BKA Join。
