数字时代，搜索引擎就是指路明灯，无处不在，不可或缺。

Elasticsearch就是一款现代化的、性能优异的、融合了搜索和分析的搜索引擎。


数据通常有两种结构：

1. 结构化数据：遵循预定义的模式，比如数据库。

1. 查询结构化数据得到的结果是精确匹配的，要么有结果，要么没有。

1. 非结构化数据：自由化、无组织的格式：

1. 相关性是指搜索引擎的结果与用户查询的匹配程度，搜索引擎使用相关性算法来确定哪些文档与用户的查询密切相关。
2. 非结构化数据是无组织的，不遵循任何模式或格式，没有预定义的结构，因此不容易搜索，比如博客文章、论文、电子邮件、pdf、音视频文件这类的数据。
3. 非结构化数据，Elasticsearch提供全文搜索功能，意味着你搜索某个词，将查询所有包含了该词的文档并使用相似性算法进行评分排序返回。



# 数据库 VS 搜索引擎

传统的数据查询架构，例如MySQL：


MySQL虽然也有全文搜索，但无法在高负载的情况下提供实时的搜索性能，而Elasticsearcch则专为搜索而生。

# 现代搜索引擎

一个好的搜索引擎应具备的：

1. First-class support for full-text (unstructured) and structured data
2. Type-ahead suggestions, auto-corrections, and did-you-mean recommendations
3. Forgiveness for users’ spelling mistakes
4. Search capabilities on geolocations and GeoPoints
5. Easy scalability, either up or down, based on the fluctuating demands
6. Blazing performance, speedy indexing and search capabilities
7. Architecture that’s a high-availability and fault-tolerant distributed system
8. Support for machine learning functionalities



相关性搜索

倒排索引



主要的搜索引擎：

1. Elasticsearch
2. Apache Solr
3. Amazon's opensearch

都是基于Lucence构建的



# Elasticsearch简介

1. 开源
2. 基于Java
3. 高可用
4. 全文搜索
5. 分布式系统
6. restful风格接口
7. 精确搜索
8. 大数据
9. 性能监控、预测分析、异常检测
10. 聚合统计
11. Kibana可视化
12. 支持机器学习
13. 可观测

## 核心功能

搜索、可观测、安全


## Elastic stack

ELK是指四件套：

1. beats：数据传输器，从外部系统加载数据并将其注入Elasticsearch。

1. Filebeat：基于文件的传输
2. Metricbeat：机器和操作系统的重要数据参数如cpu和内存信息

1. logstash：开源数据处理引擎，提取多个来源的数据对其进行处理，并将其发送到目的地
2. Elasticsearch：搜索分析引擎。
3. kibana：官方UI层，实际上也是Elasticsearch的客户端
