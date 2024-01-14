# Search and speed problems

## Modern hardware

SSD优于HDD

分片数量太多，节点之间的通信开销会增加。

过多的副本也会迅速消耗内存。

## Document modeling

Elasticsearch是一个NoSQL数据库，如果数据是父子关系、嵌套关系，需要谨慎使用。

单个字段的搜索的性能比多个字段的搜索性能好，合并字段是一个考虑点。

```json
# Searching across multiple fields, it's expensive
GET programming_books1/_search
{
  "query": {
    "multi_match": {
      "query": "Elasticsearch hands-on example driven",
      "fields": ["title","synopsis"]
    }
  }
}

# Index with copy_to field
PUT programming_books2
{
  "mappings": {
    "properties": {
      "title":{
        "type": "text",
        "copy_to": "title_synopsis"
      },
      "synopsis":{
        "type": "text",
        "copy_to": "title_synopsis"
      },
      "title_synopsis":{
        "type": "text"
      }
    }
  }
}

# Indexing a book - only two fields
PUT programming_books2/_doc/1
{
  "title":"Elasticsearch in Action",
  "synopsis":"A straightforward, hands-on, example driven, action book"
}

# Match query against the title_synopsis field
GET programming_books2/_search
{
  "query": {
    "match": {
      "title_synopsis": {
        "query": "Elasticsearch hands-on example driven",
        "operator": "OR"
      }
    }
  }
}
```

## Choosing keyword types over text types

全文搜索比term-level多了文本分析，所以性能比term-level差。

不需要full-text时可以用term-level。

如果希望在text文本上进行term-level，mapping可以定义为多字段：

```json
DELETE programming_books3
# multi-field mapping
PUT programming_books3
{
  "mappings": {
    "properties": {
      "title":{
        "type": "text",
        "fields": {
          "raw":{
            "type":"keyword"
          }
        }
      }
    }
  }
}

# Searching on a keyword field
GET programming_books3/_search
{
  "query": {
    "match": {
      "title.raw": "Elasticsearch in Action"
    }
  }
}
```

# Index speed problems

写入数据时也可能遇到性能问题。

## System-generated identifiers

如果应用生成唯一id，然后查询该唯一id中在es中是否存在再写入，这会有额外的通信开销，可以使用es自带的全局id生成。

## Bulk requests

很好理解，一定的批量查询节省IO开销

## Adjusting the refresh rate

```json
# Custom refresh setting - 1 min
PUT programming_books3/_settings
{
  "index":{
    "refresh_interval":"1m"
  }
}

# Turning off refresh activity completely
PUT programming_books3/_settings
{
  "index":{
    "refresh_interval":-1
  }
}

# Index a sample document
PUT programming_books3/_doc/10
{
  "title":"Elasticsearch for Java Developers",
  "synopsis":"Elasticsearch meets Java"
}

# If you search for this - you should return no result.
# As we switched off the refresh - it shouldn't be available in search
GET programming_books3/_search
{
  "query": {
    "match": {
      "title": "Elasticsearch Java"
    }
  }
}

# Forcing refresh on the index
POST programming_books3/_refresh

# Fetch the hot threads
GET _nodes/hot_threads

# Fetch all the thread-pools 
GET _nodes/thread_pool

# Fetch all the thread-pools usign _cat API
GET /_cat/thread_pool/?v&h=name,active,size,type
```

# Unstable clusters

## Cluster is not Green

## Unassigned shards

## Disk-usage thresholds

# Circuit breakers

Elasticsearch有熔断器机制。

为了避免内存耗尽节点宕机，设置了熔断的阈值，共有六种熔断器。

1. Parent：所有的circuit breakers的内存超过了95%
2. Inflight requests：请求占用的总内存占用超过了JVM堆内存的100%
3. Request：单个请求的内存占用占了堆内存的60%
4. Field data：字段加载时内存占用超过40%
5. Accounting requests：请求累计内存
6. Script compilation：

