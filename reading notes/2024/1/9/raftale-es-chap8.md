# Overview

1. 结构化搜索：不评分，要么匹配要么不匹配
2. 非结构化搜索：根据相关性评分排序

# How does search work
当一个请求发生时：

1. 每个节点都分配了一个协调者的角色，所以当一个请求来到节点后，该节点需要路由到其他节点
2. 各个节点收到请求后，收集数据并返回给协调者节点
3. 协调者节点收集结果并排序返回给客户端

# movie sample data

```json
DELETE movies
# Create movies index
PUT movies
{
  "mappings": { 
    "properties": { 
      "title": {
        "type": "text",
        "fields": { 
          "original": {
            "type": "keyword"
          }
        }
      },
      "synopsis": {
        "type": "text"
      },
      "actors": {
        "type": "text"
      },
      "director": {
        "type": "text"
      },
      "rating": {
        "type": "half_float"
      },
      "release_date": {
        "type": "date",
        "format": "dd-MM-yyyy"
      },
      "certificate": {
        "type": "keyword"
      },
      "genre": {
        "type": "text"
      }
    }
  }
}
```

然后插入数据：https://github.com/madhusudhankonda/elasticsearch-in-action/blob/main/kibana_scripts/ch08_search_basics.txt

# Search fundamentals

## Query vs filter context



1. query context：相关性搜索匹配
2. filter context：只匹配，没有相关性搜索

filter 因为没有相关性搜索，所以性能比query好。



之前第五章讲过，bool查询包括四种：must，must_not, should, filter.

filter, must是通过filter context执行的；

所以下面的就是只匹配：

```json
GET movies/_search
{
  "query": {
    "bool": {
      "filter": [{
        "match": {
          "title": "Godfather"
        }
      }]
    }
  }
}
```

# Anatomy of a request and a response

search request


search response


# URI request searches

```
GET|POST <*your_index_name*>/_search?q=<name:value> AND|OR <name:value>
GET movies/_search**?q=title:Godfather**
GET movies/_search?q=title:Godfather Knight Shawshank
GET movies/_search?q=Knight AND actors:Bale
GET movies/_search?q=Knight OR actors:Bale
```

URI用的不多
