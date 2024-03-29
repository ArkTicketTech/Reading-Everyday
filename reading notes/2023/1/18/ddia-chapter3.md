---
layout: "post"
title: "Design Data Intensive Application"
date: "2023-01-18 10:35"
tags:
  - DDIA读书笔记
---

# 第三章 数据的存储和检索

数据库主要做两件事，当你给它一些数据时，它应该存储数据，当你稍后再次询问它时，它应该把数据还给你。在第二章，主要是讲解了抽象化地数据库模型和查询语言，本章将从具象化的底层讲解我们如何存储给定的数据，以及如何在需要时再次找到它。

不同的数据库基于不同的数据引擎，适应特定数据查询场景，从而尽可能快速地找到目标数据，即**使用场景**→ **查询类型** → **存储格式**

根据数据的查询类型可以将数据库分为两种类型：

**OLTP**，也叫联机事务处理（Online Transaction Processing），表示事务性非常高的系统，一般都是高可用的在线系统，以小的事务以及小的查询为主，评估其系统的时候，一般看其每秒执行的Transaction以及Execute SQL的数量。

**OLAP**，也叫联机分析处理（Online Analytical Processing）系统，有的时候也叫DSS决策支持系统，就是我们说的数据仓库。在这样的系统中，语句的执行量不是考核标准，因为一条语句的执行时间可能会非常长，读取的数据也非常多。

<img src="/Users/daniel/Project/danie10ne.github.io/_posts/DDIA/ddia-chapter3.png" alt="ddia-chapter3" style="zoom: 33%;" />

<img src="/Users/daniel/Project/danie10ne.github.io/_posts/DDIA/OLTP-ETL-and-OLAP-layers.png" alt="OLTP-ETL-and-OLAP-layers" style="zoom: 50%;" />

## 驱动数据库的底层数据结构

