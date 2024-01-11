The term level search

1. understanding term-level queries
2. term-level queries in action

# Overview of term-level search

## term-level queries are not analyzed

term-level 搜索不会经过analyze，不会评分，但要注意如果是对text字段进行term-level query，由于text可能会经过standard analyzer，值的大小写可能改变，比如Java变成java，所以text用于精确搜索可能造成结果不准确。term-level query适用于keyword类型的字段。



## term-level query example

如图：



# the term query

term-level查询关键字：term, terms, ids, fuzzy

```json
# Fetching the movies with a specific rating
GET movies/_search
{
  "query": {
    "term": {
      "certificate": "R"
    }
  }
}
```

## the term query on text fields

term query一般不在text字段上用，但是如果text是确定的枚举，则可以使用，但一般还是尽量不要用。

## example term query

```json
# This term query wouldn't return the results
GET movies/_search
{
  "query": {
    "term": {
      "title": "The Godfather" 
    }
  }
}
```

title是text字段，所以会被拆分为`[the, godfather]`，该查询是查询不到数据的。

## shortened term-level queries

之前的语法都是简化版的，如果要添加其他参数比如权重boost，语法为：

```json
GET movies/_search
{
  "query": {
    "term": {
      "title": {
        "value" : "The Godfather" ,
        "boost": 2
      }
    }
  }
}
```

# the terms query

单字段多值查询：

```json
# Terms query
GET movies/_search
{
  "query": {
    "terms": { 
      "certificate": ["PG-13","R"]
    }
  }
}
```

certificate中"PG-13"和"R"是or的关系。

## the terms lookup query

我们希望获取所有Steven Spielberg导演的电影，但我不想直接用Steven Spielberg查询。

```json
PUT classic_movies
{
  "mappings": {
    "properties": {
      "title": {
        "type": "text"
      },
      "director": {
        "type": "keyword"
      }
    }
  }
}

# Index three docs
PUT classic_movies/_doc/1
{
  "title":"Jaws",
  "director":"Steven Spielberg"
}

PUT classic_movies/_doc/2
{
  "title":"Jaws II",
  "director":"Jeannot Szwarc"
}

PUT classic_movies/_doc/3
{
  "title":"Ready Player One",
  "director":"Steven Spielberg"
}


# Search for terms with director matching with the "path" value 
# from "classic_movies" index for a document with an ID as 3 (which is "Steven Spielberg)
GET classic_movies/_search
{
  "query": {
    "terms": {
      "director": {
        "index":"classic_movies",
        "id":"3",
        "path":"director"
      }
    }
  }
}
```

先查询出id为3的数据，director为Steven Spielberg，然后查出所有的director为Steven Spielberg的文档返回。



这个功能似乎可以扩展到很多引用场景，后续再研究。

# the ids query

```json
# IDs query
GET movies/_search
{
  "query": {
    "ids": {
      "values": [10,4,6,8]
    }
  }
}

# Terms query fetching IDs
GET movies/_search
{
  "query": {
    "terms": {
    "_id":[10,4,6,8]
    }
  }
}
```

# the exists query

1. 判断某个字段是否存在

```json
# Exists query
GET movies/_search
{
  "query": {
    "exists": {
      "field": "_id"
    }
  }
}
GET movies/_search
{
  "query": {
    "exists": {
      "field": "title"
    }
  }
}
```

# the range query

```json
# Range query
GET movies/_search
{
  "query": {
    "range": {
      "rating": {
        "gte": 9.0,
        "lte": 9.5
      }
    }
  }
}

GET movies/_search
{
  "query": {
    "range": {
      "release_date": {
        "gte": "01-01-1970"
      }
    }
  },
  "sort": [
    {
      "release_date": {
        "order": "asc"
      }
    }
  ]
}
```

# the wildcard query

通配符查询：

1. asterisk(*): searches for zero or more characters
2. question mark(?): searches for single character

通配符查询一般用在text字段，如果用在keyword字段上，也是可以的，但要注意keyword本身只有一个word，所以通配符要写好。



```json
# Wildcard Query with a wildcard at the end
GET movies/_search
{
  "query": {
    "wildcard": { 
      "title": {
        "value": "god*"
      }
    }
  }
}

# Wildcard Query with a wildcard in middle
GET movies/_search
{
  "_source": false, 
  "query": {
    "wildcard": { 
      "title": {
        "value": "g*d"
      }
    }
  },
  "highlight": {
    "fields": {
      "title": {}
    }
  }
}

GET movies/_search
{
  "query": {
    "wildcard": { 
      "title.original": {
        "value": "The God*"
      }
    }
  },
  "highlight": {
    "fields": {
      "title": {}
    }
  }
}
```



range, prefix, fuzzy, regex, join, wildcard查询都是比较昂贵的，需要谨慎使用

# the prefix query

```json
# Prefix query 
GET movies/_search
{
  "query": {
    "prefix": {
      "genre.original": {
        "value": "Ad"
      }
    }
  }
}
```

## Shortened queries

```json
GET movies/_search
{
  "query": {
    "prefix": {
      "actors.original":  "Leo"
    }
  } 
}
```

## Speed up prefix queries

prefix查询时需要进行计算prefix的，如果可以预先写到倒排索引中，查询就可以提供。

方法是通过设置`index_prefixes`，它会有前缀数量的最小和最大字符数量。

```json
#Index prefixes
PUT boxoffice_hit_movies
{
  "mappings": {
    "properties": {
      "title":{
        "type": "text",
        "index_prefixes":{
          "min_chars":2, 
          "max_chars":10 
        }
      }
    }
  }
}

PUT boxoffice_hit_movies/_doc/1
{
  "title":"Gladiator"
}

GET boxoffice_hit_movies/_search
{
  "query": {
    "prefix": {
      "title": {
        "value": "gla"
      }
    }
  }
}
```

# fuzzy queries

拼写错误对于人类来说很正常，系统应该支持拼写错误的查询 。

Fuzziness is a process of searching for similar terms based on the *Levenshtein distance algorithm* (also referred to as the *edit distance*) 。

Levenshtein距离是编辑距离的一种，指两个子串之间，由一个转换成另一个所需要的最少编辑操作次数。

Fuzziness实际上就是编辑距离，设置fuzziness为n，就说明允许编辑距离为n。



```json
# Fuzzy query with edit distance of 1
GET movies/_search
{
  "query": {
    "fuzzy": {
      "genre": {
        "value": "rama",
        "fuzziness": 1
      }
    }
  },
  "highlight": {
    "fields": {
      "genre": {}
    }
  }
}
```

fuzziness的默认配置：

1. word length 为0-2， fuzziness为0
2. word length 为3-5， fuzziness为1
3. word length 为5以上， fuzziness为2

测试了下，似乎超过了3就不准确了。
