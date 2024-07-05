性能比较

参数	SATA HDD (Seagate 16TB)	SATA SSD (Intel S4510)	NVMe SSD (Intel P4610)
接口类型	SATA III	SATA III	NVMe PCIe 3.0 x4
容量	16 TB	240 GB - 3.84 TB	1.6 TB - 7.68 TB
顺序读取速度	250 MB/s	560 MB/s	3200 MB/s
顺序写入速度	250 MB/s	510 MB/s	3000 MB/s
随机读取 IOPS	170 IOPS	95,000 IOPS	637,000 IOPS
随机写入 IOPS	440 IOPS	36,000 IOPS	620,000 IOPS
延迟	4-5 ms	< 50 µs	< 20 µs
功耗（活动时）	8-9 W	3.6 W	18 W
功耗（闲置时）	5 W	1.1 W	5 W
可靠性（MTBF）	2.5 million hours	2 million hours	2 million hours

写入寿命：TBW / DWPD 
消费级典型：2TB的盘有 1200 TBW
企业级典型： 1 DWPD  6TB的盘， 6TB/day * 365day * 质保 5年 = 10950TBW
对于机械盘，没有明确的写入寿命。

SATA + HDD vs NVMe 协议
SATA (AHCI）只支持单队列、queue depth = 32。与HDD的“电梯调度”算法相匹配。