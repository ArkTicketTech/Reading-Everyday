# Staring into the Abyss: An Evaluation of Concurrency Control with One Thousand Cores

## Abstract

随着计算结构往多核方向发展，DBMS 因为并发控制的问题难以利用 CPU 多核心的能力增强速度。本文就 DBMS 在多核心 CPU 上的并发控制进行试验，做出了以下探索：

1. 对七种并发控制方案在多核心 CPU 上的内存数据库上进行了可扩展性评估，并总结了不同并发控制方案的瓶颈所在；
2. DBMS 可能需要重新设计其体系结构来适应多核 CPU 架构带来的提速。

##  Introduction

计算机架构现状：

1. 摩尔定律逐渐失效，CPU 的单核时钟频率增长受限，CPU 转向多核心架构；

2. 乱序，超标量处理器正在被  in-order、single issue 所取代；

3. 指令级别并行和单一线程正在向多线程并行扩展。
