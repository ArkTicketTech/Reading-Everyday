# Query DSL

## Sample query

`multi_search`多字段查询同一个文本。

```json
# Query DSL sample query
GET movies/_search
{
  "query": {
    "multi_match": {
      "query": "Lord",
      "fields": ["synopsis","title"]
    }
  }
}
```

## Query DSL for cURL

```json
curl -XGET "http://localhost:9200/movies/_search" -H
'Content-Type: application/json' -d'
{
  "query": {
    "multi_match": {
      "query": "Lord",
      "fields": ["synopsis","title"]
    }
  } 
}'
```

## Query DSL for aggregations

获取平均分：

```json
GET movies/_search
{
  "size": 0, 
  "aggs": {
    "average_movie_rating": {
      "avg": {
        "field": "rating"
      }
    }
  }
}
```

## Leaf and compound queries

1. leaf queries：单字段单一查询
2. compound queries：比如带bool查询

# search features

## Pagination

分页查询

```json
# Paginated results using size and from
GET movies/_search
{
  "size": 100, 
  "from": 3, 
  "query": {
    "match_all": {}
  }
}
```

## Highlighting

返回结果 重点标注

## Explaining relevancy scores

相关性解释可以通过_search中的explain flag或者_explain API。

```json
# Explanation
GET movies/_search
{
  "explain": true, 
  "_source": false, 
  "query": {
    "match": {
      "title": "Lord"
    }
  }
}

# Exactly same as above - explains the scores
GET movies/_explain/14
{
  "query":{
    "match": {
      "title": "Lord"
    }
  }
}
```


The relevancy score is calculated by multiplying three components:

1. the inverse document frequency (IDF)：

1. 计算公式，`log(1 + (N - n + 0.5) / (n + 0.5))`
2. n, number of documents containing term。包含该字段的文档
3. N, total number of documents with field。包含该字段的文档数，一般为总数

1. the term frequency (TF)

1. 计算公式：`tf, computed as freq / (freq + k1 * (1 - b + b * dl / avgdl)) from:`
2. freq, occurrences of term within document
3. k1, term saturation parameter

1. the boost factor.



上面的公式怎么来的，现在不作考究，后续深入再研究。



尽量使用_explain API，当不匹配的时候会返回原因，且避免查询的性能开销。

## Sorting

结果默认按score进行排序，但我们也可以根据某个字段进行排序。

```json
# Sort by rating in ascending order
GET movies/_search
{
  "size": 10, 
  "query": {
    "match": {
      "title": "Godfather"
    }
  },
  "sort": [{
    "rating": {"order": "asc"}
    }
  ]
}
```

当我们用一个额外的字段进行排序时，score会被设置为null，Elasticsearch并不会计算score；但如果实在需要评分，可以在查询的时候加上`track_scores:true`。

```json
GET movies/_search
{
  "size": 10, 
  "track_scores":true,
  "query": {
    "match": {
      "title": "Godfather"
    }
  },
  "sort": [{
    "rating": {"order": "asc"}
    }
  ]
}
```

多字段排序：

```json
GET movies/_search
{
  "track_scores":true,
  "size": 10, 
  "query": {
    "match": {
      "genre": "crime"
    }
  },
  "sort": [
    {"rating":{"order":"asc"}},
    {"release_date":{"order":"asc"}}
  ]
}
```

## Manipulating results

```json
# Selected fields
GET movies/_search
{
  "_source": false, 
  "query": {
    "match": {
      "certificate": "R"
    }
  },
  "fields": [
    "title*",
    "rating"
  ]
}

# Source filtering
GET movies/_search
{
  "_source": ["title*","synopsis", "rating"], 
  "query": {
    "match": {
      "certificate": "R"
    }
  }
}

# Source filtering with a script field
GET movies/_search
{
  "_source": ["title*","synopsis", "rating"], 
  "query": {
    "match": {
      "certificate": "R"
    }
  },
  "script_fields": {
    "top_rated_movie": {
      "script": {
        "lang": "painless",
        "source": "if (doc['rating'].value > 9.0) 'true'; else 'false'"
      }
    }
  }
}

# Source filtering using includes and excludes
GET movies/_search
{
  "_source": {
    "includes": ["title*","synopsis","genre"],
    "excludes": ["title.original"]
  }, 
  "query": {
    "match": {
      "certificate": "R"
    }
  }
}

# Return everything except synoposis and actors
GET movies/_search
{
  "_source": {
    "excludes": ["synopsis","actors"]
  }, 
  "query": {
    "match": {
      "certificate": "R"
    }
  }
}
```

## Searching across indexes and data streams

```json
## Searching across multiple indices
# Searching for Pacino movies across all indicies
GET _search
{
  "query": {
    "match": {
      "actors": "Pacino"
    }
  }
}

# Using _all to indicate every index in the cluster
GET _all/_search
{
  "query": {
    "match": {
      "actors": "Pacino"
    }
  }
}

# Use a wildcard to denote every index
GET */_search
{
  "query": {
    "match": {
      "actors": "Pacino"
    }
  }
}

GET movies_new/_mapping
GET movies*/_search
{
  "indices_boost": [
    { "movies": 0.1},
    { "movies_new": 0},
    { "movies_top": 2.0}
  ], 
  "query": {
    "match": {
      "title": "Redemption"
    }
  }
}
```
