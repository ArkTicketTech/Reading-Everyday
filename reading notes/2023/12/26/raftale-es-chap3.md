1. High-level architecture and Elasticsearch’s building blocks
2. Search and indexing mechanics
3. How an inverted index works
4. Relevancy and similarity algorithms
5. Routing algorithm

# A high-level overview

Elasticsearch是构建在Lucene上的，Lucene是一个高性能的，全文搜索的library。


## 传输数据

数据可来源于

1. database：数据库
2. filestore：数据写入日志文件的情况，
3. applications：流处理，一般为消息



## 处理数据

索引是逻辑上的概念，等同于mysql中的表。

数据类型包括：dates, numbers, strings, booleans, IP addresses, locations, and so on。

通过mapping，Elasticsearch将JSON转换为合适的数据类型。



text的字段需要经历全文分析的过程，文本分析会经历两个过程：

1. tokenization：一个文本拆分成许多token
2. normalization：丰富token，增加其他的token含义，比如peppers可以与capcicum关联。例如同义词

