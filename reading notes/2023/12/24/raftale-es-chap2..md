# Term-level queries

专门指结构化数据的查询。

非文本字段是不会被分割为token的，也就没有分词，存储的就是其本身。

term-level 查询其实就是说查询要么有匹配上的结果，要么没有匹配上的结果。

## the term query



```json
GET books/_search
{
  "_source": ["title","edition"], 
  "query": {
    "term": { 
      "edition": { 
        "value": 3
      }
    }
  }
}
```



## the range query

范围查询：

```json
GET books/_search
{
  "query": {
    "range": { 
      "amazon_rating": {
        "gte": 4.5,
        "lte": 5
      }
    }
  }
}
```

# Compound queries

查询方式：

1. Boolean (bool) query
2. Constant score (constant_score) query
3. Function (function_score) score,
4. Boosting (boosting) query
5. Disjunction max (dis_max) query

## boolean query

boolean查询使用最多，bool条件：

1. must：必须满足，有得分
2. must not：必须不满足，
3. should：可以不满足，但如果满足会很加分
4. filter：必须满足，没有得分

```json
“GET books/_search
{
  "query": {
    "bool": {
      "must": [{ }],
      "must_not": [{ }],
      "should": [{ }],
      "filter": [{ }]
    }
  }
}
```



接下来我们用这三个条件组合查询：

1. authored by Joshua
2. having a rating greater than 4.7
3. published after 2015



## must query

```json
GET books/_search
{
	"query": {
		"bool": {
			"must": [
				{ 
					"match": {
						"author": "Joshua Bloch"
					}
				},
				{
					"match_phrase": {
						"synopsis": "best Java programming books"
					}
				}
			]
		}
	}
```

## must not clause

```json
GET books/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "author": "Joshua"
          }
        }
      ],
      "must_not": [
        {
          "range": {
            "amazon_rating": {
              "lt": 4.7
            }
          }
        }
      ]
    }
  }
}
```

## the should clause

should 类似于or，如果匹配上的话，将会提高结果的相关性。

```json
GET books/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "author": "Joshua"
          }
        }
      ],
      "should": [
        {
          "match": {
            "tags": "Software"
          }
        }
      ]
    }
  }
}
```

## the filter clause

filter进行条件过滤必须匹配，但不影响score，而must则会影响score。

```json
GET /books/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "author": "Joshua"
          }
        }
      ],
      "should": [
        {
          "match": {
            "tags": "Software"
          }
        }
      ],
      "filter": [
        {
          "range": {
            "release_date": {
              "gte": "2015-01-01"
            }
          }
        }
      ]
    }
  }
}
```

# Aggregations

分析总结，有三大类

1. metric aggregations：like sum, min, max, avg
2. bucket aggregations：Bucket aggregations help collect data into buckets, segregated by intervals like days, age groups, etc. These help us build histograms, pie charts and other visualizations.
3. pipeline aggregations：Pipeline aggregations work on the output from the other aggregations.



数据准备：https://github.com/madhusudhankonda/elasticsearch-in-action/blob/main/datasets/covid-26march2021.txt

```json
POST covid/_bulk
{"index":{}}
{"country":"United States of America","date":"2021-03-26","cases":30853032,"deaths":561142,"recovered":23275268,"critical":8610}
{"index":{}}
{"country":"Brazil","date":"2021-03-26","cases":12407323,"deaths":307326,"recovered":10824095,"critical":8318}
{"index":{}}
{"country":"India","date":"2021-03-26","cases":11908373,"deaths":161275,"recovered":11292849,"critical":8944}
{"index":{}}
{"country":"Russia","date":"2021-03-26","cases":4501859,"deaths":97017,"recovered":4120161,"critical":2300}
{"index":{}}
{"country":"France","date":"2021-03-26","cases":4465956,"deaths":94275,"recovered":288062,"critical":4766}
{"index":{}}
{"country":"United kingdom","date":"2021-03-26","cases":4325315,"deaths":126515,"recovered":3768434,"critical":630}
{"index":{}}
{"country":"Italy","date":"2021-03-26","cases":3488619,"deaths":107256,"recovered":2814652,"critical":3628}
{"index":{}}
{"country":"Spain","date":"2021-03-26","cases":3255324,"deaths":75010,"recovered":3016247,"critical":1830}
{"index":{}}
{"country":"Turkey","date":"2021-03-26","cases":3149094,"deaths":30772,"recovered":2921037,"critical":1810}
{"index":{}}
{"country":"Germany","date":"2021-03-26","cases":2754002,"deaths":76303,"recovered":2467600,"critical":3209}
```

## Metrics



![img](https://cdn.nlark.com/yuque/0/2023/png/32473878/1703511456363-c1860b61-c4d3-4ffa-bfdb-b183cee629a9.png)

```json
GET covid/_search
{
  "size": 0, 
  "aggs": { 
    "critical_patients": { 
      "sum": { 
        "field": "critical" 
      }
    }
  }
}
```

默认返回所有参与统计的结果，`"size": 0`是为了不返回参与统计的记录，只返回聚合后的信息.

`critical_patients`是自定义的字段，结果将返回到这个字段上。

```json
{
  "took" : 2,
  "timed_out" : false,
  "_shards" : {
    "total" : 1,
    "successful" : 1,
    "skipped" : 0,
    "failed" : 0
  },
  "hits" : {
    "total" : {
      "value" : 10,
      "relation" : "eq"
    },
    "max_score" : null,
    "hits" : [ ]
  },
  "aggregations" : {
    "critical_patients" : {
      "value" : 44045.0
    }
  }
}
```

有趣的是，`stats`会返回`count, min, max, avg, sum`

```json
GET covid/_search
{
  "size": 0,
  "aggs": {
    "all_stats": {
      "stats": {
        "field": "deaths"
      }
    }
  }
}
```



```json
{
  "took" : 3,
  "timed_out" : false,
  "_shards" : {
    "total" : 1,
    "successful" : 1,
    "skipped" : 0,
    "failed" : 0
  },
  "hits" : {
    "total" : {
      "value" : 10,
      "relation" : "eq"
    },
    "max_score" : null,
    "hits" : [ ]
  },
  "aggregations" : {
    "all_stats" : {
      "count" : 10,
      "min" : 30772.0,
      "max" : 561142.0,
      "avg" : 163689.1,
      "sum" : 1636891.0
    }
  }
}
```

## Bucket Aggregations

bucket简单理解就是分组

聚合就是分组统计，在Es中可以开箱即用

### histogram buckets

使用2500的bucket对critical patients的数量分组统计：

```json
GET covid/_search
{
  "size": 0,
  "aggs": {
    "critical_patients_as_histogram": { 
      "histogram": { 
        "field": "critical", 
        "interval": 2500 
      }
    }
  }
}
```

输出：

```json
{
  "took" : 14,
  "timed_out" : false,
  "_shards" : {
    "total" : 1,
    "successful" : 1,
    "skipped" : 0,
    "failed" : 0
  },
  "hits" : {
    "total" : {
      "value" : 10,
      "relation" : "eq"
    },
    "max_score" : null,
    "hits" : [ ]
  },
  "aggregations" : {
    "critical_patients_as_histogram" : {
      "buckets" : [
        {
          "key" : 0.0,
          "doc_count" : 4
        },
        {
          "key" : 2500.0,
          "doc_count" : 3
        },
        {
          "key" : 5000.0,
          "doc_count" : 0
        },
        {
          "key" : 7500.0,
          "doc_count" : 3
        }
      ]
    }
  }
}
```

### range buckets

```json
GET covid/_search
{
  "size": 0, 
  "aggs": {
    "range_countries": {
      "range": { 
        "field": "deaths", 
        "ranges": [ 
          {"to": 60000},
          {"from": 60000,"to": 70000},
          {"from": 70000,"to": 80000},
          {"from": 80000,"to": 120000}
        ]
      }
    }
  }
}
```



```json
{
  "took" : 12,
  "timed_out" : false,
  "_shards" : {
    "total" : 1,
    "successful" : 1,
    "skipped" : 0,
    "failed" : 0
  },
  "hits" : {
    "total" : {
      "value" : 10,
      "relation" : "eq"
    },
    "max_score" : null,
    "hits" : [ ]
  },
  "aggregations" : {
    "range_countries" : {
      "buckets" : [
        {
          "key" : "*-60000.0",
          "to" : 60000.0,
          "doc_count" : 1
        },
        {
          "key" : "60000.0-70000.0",
          "from" : 60000.0,
          "to" : 70000.0,
          "doc_count" : 0
        },
        {
          "key" : "70000.0-80000.0",
          "from" : 70000.0,
          "to" : 80000.0,
          "doc_count" : 2
        },
        {
          "key" : "80000.0-120000.0",
          "from" : 80000.0,
          "to" : 120000.0,
          "doc_count" : 3
        }
      ]
    }
  }
}
```
