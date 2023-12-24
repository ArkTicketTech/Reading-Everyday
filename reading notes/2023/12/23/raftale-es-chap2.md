# Priming with data

storing data into Elasticsearch.



## An online bookstore

不需要预定义数据结构，可以直接添加数据。

初始化数据：

```json
POST _bulk
{"index":{"_index":"books","_id":"1"}}
{"title": "Core Java Volume I – Fundamentals","author": "Cay S. Horstmann","edition": 11, "synopsis": "Java reference book that offers a detailed explanation of various features of Core Java, including exception handling, interfaces, and lambda expressions. Significant highlights of the book include simple language, conciseness, and detailed examples.","amazon_rating": 4.6,"release_date": "2018-08-27","tags": ["Programming Languages, Java Programming"]}
{"index":{"_index":"books","_id":"2"}}
{"title": "Effective Java","author": "Joshua Bloch", "edition": 3,"synopsis": "A must-have book for every Java programmer and Java aspirant, Effective Java makes up for an excellent complementary read with other Java books or learning material. The book offers 78 best practices to follow for making the code better.", "amazon_rating": 4.7, "release_date": "2017-12-27", "tags": ["Object Oriented Software Design"]}
...
```

### Document  APIS

### using cURL

### indexing document

```json
PUT books/_doc/2
{
  "title":"Core Java Volume I - Fundamentals",
  "author":"Cay S. Horstmann",
  "release_date":"2018-08-27",
  "amazon_rating":4.8,
  "best_seller":true,
  "prices": {
    "usd":19.95,
    "gbp":17.95,
    "eur":18.95
  }
}
```

返回：

```json
{
  "_index" : "books",
  "_type" : "_doc",
  "_id" : "2",
  "_version" : 1,
  "result" : "created",
  "_shards" : {
    "total" : 2,
    "successful" : 1,
    "failed" : 0
  },
  "_seq_no" : 1,
  "_primary_term" : 1
}
```

一个请求发出后，它的流程大致为：

1. Kibana请求Elasticsearch server
2. server接收到请求：

1. 分析并处理文档数据
2. 创建数据结构：倒排索引、BKD树
3. 存储数据到内存中
4. 持久化到文件系统
5. 返回响应给客户端

1. Kibana收到响应


# 查询数据

查询文档总数：

```bash
GET /books/_count
```

查询文档：



1. 查询单条文档：`GET books/_doc/1`


如果不想返回元数据：`GET /books/_source/1`

1. 根据IDs查询多条文档：


```json
GET /books/_search
{
  "query": {
    "ids" : {
      "values": [1, 2, 3]
    }
  }
}
```

如果我们不想要结果，只想要分数，不返回source。

```json
GET /books/_search
{
  "_source": false, 
  "query": {
    "ids" : {
      "values": [1, 2, 3]
    }
  }
}
```



1. 查询所有文档

```
GET /books/_search
```

或者

```json
GET /books/_search
{
  "query": {
    "match_all": {
      
    }
  }
}
```

如果想让结果的score都为2：

```json
GET /books/_search
{
  "query": {
    "match_all": {
      "boost": 2
    }
  }
}
```



# 全文搜索

非结构化的文本就是指的full text

## Match Query: books by an author

我们想搜索一个叫Joshua的人写的书，尽管他的全称是Joshua Bloch。

```json
GET books/_search
{
  "query": {
    "match": {
      "author": "Joshua"
    }
  }
}
```

能查询出来是因为倒排索引。

Joshua无论如何大小写都是能够查询出来的，但是如果查询Josh就无法查询出来了，因为分词的最小粒度就是Joshua了。如果要通过Josh查询出来，可以使用`prefix query`。

```json
GET books/_search
{
  "query": {
    "prefix": {
      "author": "josh"
      }
    }
}
```

prefix是精确查询，必须要区分大小写。



## Match query with operator

如果查询是"Joshua Doe"，你期望返回什么？

事实上是`"Joshua Doe"`这个作者的，那如果返回为空合理吗？

其实用户仍然希望可以返回Joshua写的书，万一是用户记错了全称呢？所以查询`"Joshua Doe"`，我们的搜索条件其实是`Joshua or Doe`。

所以我们查询`Joshua Schildt`，会返回两条记录，分别是`Joshua Bloch`和`Herbert Schildt`写的。

```json
GET books/_search
{
  "query": {
   "match": {
     "author": "Joshua Schildt"
   }
  }
}
```

但如果我们真的确认作者的全名，就像查询该作者，也可以这样查询：

```json
GET books/_search
{
  "query": {
   "match": {
     "author": "Joshua Schildt",
     "operator":"AND"
   }
  }
}
```

## Indexing documents using the _bluk API

批量写操作，格式为

```json
POST books/_bulk
{"index":{"_id":"4"}}
{"title":"Effective Java","author":"Joshua Bloch","release_date":"2001-06-01","amazon_rating":4.7,"best_seller":true,"prices":{"usd":9.95,"gbp":7.95,"eur":8.95}}
{"index":{"_id":"5"}}
{"title":"Effective Java","author":"Joshua Bloch","release_date":"2001-06-01","amazon_rating":4.7,"best_seller":true,"prices":{"usd":9.95,"gbp":7.95,"eur":8.95}}
```

或者是：

```json
POST _bulk
{"index":{"_index":"books", "_id":"6"}}
{"title":"Effective Java","author":"Joshua Bloch","release_date":"2001-06-01","amazon_rating":4.7,"best_seller":true,"prices":{"usd":9.95,"gbp":7.95,"eur":8.95}}
{"index":{"_index":"books", "_id":"7"}}
{"title":"Effective Java","author":"Joshua Bloch","release_date":"2001-06-01","amazon_rating":4.7,"best_seller":true,"prices":{"usd":9.95,"gbp":7.95,"eur":8.95}}
```

## Searching across multiple fields

多字段查询：如果我们要查的文本不止在一个字段中

```json
GET /books/_search
{
  "query": {
    "multi_match":{
      "query":"Java",
      "fields":["title", "synopsis"]
    }
  }
}
```

全文搜索的结果都有一个得分，在`_score`字段中。得分意味着查询与结果之间的相关性，返回结果按得分从大到小排序。

## Boosting results

如果多字段查询时要给某个字段更高的优先级（更高的相关性），比如查询内容在标题中比在摘要中显然更为匹配。

title提高3倍：title^3

```json
GET /books/_search
{
  "query": {
    "multi_match":{
      "query":"Java",
      "fields":["title^3", "synopsis"]
    }
  }
}
```

## Search phrases

有时我们希望查询 包含了某句话的结果，就可以用`match_phrase`：

```json
GET books/_search 
{
  "query":{
    "match_phrase":{
      "synopsis":"must-have book for every Java programmer"
    }
  }
}
```

## Highlighting the results

如果要高亮显示查询匹配的文本，可以

```json
GET books/_search 
{
  "query":{
    "match_phrase":{
      "synopsis":"must-have book for every Java programmer"
    }
  },
  "highlight": {
    "fields": {
      "synopsis": {}
    }
  }
}
```

## 2Phrases with missing words

前面说到`match_phrase`是必须包含整个句子，但如果少两个单词也想查询到，slop参数可以支持，slop就代表缺失的参数个数，如果缺少2个参数，那么slop大于等于2就能查询出来。

```json
GET /books/_search 
{
  "query":{
    "match_phrase":{
      "synopsis": {
        "query":"must-have book Java programmer",
        "slop":2
      }
    }
  }
}
```

## Honoring spelling mistakes

有时查询需要包容拼写错误，Elasticsearch使用了Levenshtein Edit's distance算法来做近似匹配，Levenshtein Edit's distance是指两个字符串之间，由一个转换成另一个字符串的最小编辑操作次数，一般是用动态规划的方法解决。

Elasticsearch使用fuzziness来定义可以包容的拼写错误的字母个数。

```json
GET /books/_search
{
  "query": {
    "match": {
      "tags": {
        "query": "Komputer",
        "fuzziness": 1
      }
    }
  }
}
```
