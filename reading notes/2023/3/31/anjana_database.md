**Databases in 2022: A Year in Review**

**Big Database Funding Has Slowed Big Time**

2021: a banner year for database funding

The market cannot sustain so many independent software vendors (ISVs) for databases.

Gartner analysts predict that 50% of independent DBMS vendors will go out of business by 2025.

**Blockchain Databases Are Still a Stupid Idea**

The core tenet of the web3 movement is storing state in a blockchain database.

Blockchains are essentially decentralized log-structured databases(i.e., ledger) that maintain incremental checksums using some variation of Merkle trees and a BFT consensus protocol to determine the next update to install into the database.

区块链实质上是：去中心化的日志结构数据库。

从Proof-of-Work(PoW) 转换为更energy-intensive的Proof-of-Stake(PoS)共识机制的确提高了区块链数据库的性能。

**New Database Systems**

Google AlloyDB: 

- a modified version of PostgreSQL that separates its compute and storage layers
- support WAL processing directly in storage

AlloyDB’s architecture is similar to Amazon Aurora and Neon, where the DBMS storage has an additional compute layer to process WAL records independently of the compute nodes.

Velox:

* as a new execution engine for PrestoDB in 2020.
* C++ extensible execution engine with a memory pool and storage connectors. 

in the cloud: the storage layer is the same for everyone(e.g., Amazon controls EBS/S3)

differentiator: UI/UX stuff and query optimization

**The Loss of a Database Pioneer**

using MMAP in MonetDB was the right choice

**Using A Database Fortune to Save Democracy**