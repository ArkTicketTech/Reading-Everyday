# Coompound queries

前面讲到的查询大多都是leaf queries，基于单个字段的。

但现实中查询是非常复杂的，可能基于各种条件的组合。

复合查询的格式：

```json
GET <index>/_search
{
  "query": {
    "bool | constant_score | boosting | dis_max | function_score" {
      //search_query
    }
  }
}
```

1. bool：各种条件的组合查询
2. constant_score：
3. boosting
4. dis_max
5. function

# The Boolean(bool) query

1. must: and
2. must_not: not
3. should：or
4. filter：不带评分的must

## The bool query structure

```json
GET <index>/_search
{
  "query": {
    "bool" {
      "must": [{}],
      "must_not": [{}],
      "should": [{}],
      "filter": [{}]
    }
  }
}


GET <index>/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "FIELD": "TEXT"
          }
        },
        {
          "term": {
            "FIELD": {
              "value": "value"
            }
          }
        }
      ],
      "must_not": [
        {
          "bool": {
            "must": [
              {}
            ]
          }
        }
      ],
      "should": [
        {
          "range": {
            "FIELD": {
              "gte": 10,
              "lte": 20
            }
          }
        },
        {
          "terms": {
            "FIELD": [
              "VALUE1",
              "VALUE2"
            ]
          }
        }
      ],
      "filter": [
        {}
      ]
    }
  }
}
```

实际上在上面must_not中还可以嵌套bool复合查询。

## The must clause

Enhancing the must clause

```json
# TVs in a price range with a particular colour
GET products/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "product": "TV"
          }
        },
        {
          "range": {
            "price": {
              "gte": 1000,
              "lte": 2000
            }
          }
        }, 
        {
          "terms": {
            "colour": [
              "silver",
              "black"
            ]
          }
        }
      ]
    }
  }
}
```



## The must_not clause

```json
# Enhancing the must_not queries
GET products/_search
{
 "query": {
   "bool": {
     "must_not": [
       {
         "terms": {
           "brand.keyword": [
             "Philips",
             "Samsung"
           ]
         }
       },
       {
         "range": {
           "user_ratings": {
             "lte": 4.0
           }
         }
       },
       {
         "match": {
           "colour": "black"
         }
       }
     ],
     "must": [
       {
         "match": {
           "product": "TV"
         }
       },
       {
         "term": {
           "resolution": {
             "value": "4k"
           }
         }
       },
       {
         "range": {
           "price": {
             "gte": 500,
             "lte": 700
           }
         }
       }
     ]
   }
 }
}
```

## The should clause

```json
# This will fetch eihter brand matching "LG" or product "TV"
GET products/_search
{
  "_source": ["product","brand", "overview","price"], 
  "query": {
    "bool": {
      "should": [
        {
          "range": {
            "price": {
              "gte": 500,
              "lte": 1000
            }
          }
        },
        {
          "match_phrase_prefix": {
            "overview": "4k ultra hd"
          }
        }
      ]
    }
  }
}
```

## The filter clause

```json
# Filter clause with must
GET products/_search
{
  "_source": ["brand","product","colour","price"], 
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "brand": "LG"
          }
        }
      ], 
      "filter": [
        {
          "range": {
            "price": {
              "gte": 500,
              "lte": 1000
            }
          }
        }
      ]
    }
  }
}
```

## Combing all the clauses

```json
#All clauses combined
GET products/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "brand": "LG"
          }
        }
      ],
      "must_not": [
        {
          "term": {
            "colour": "silver"
          }
        }
      ],
      "should": [
        {
          "match": {
            "energy_rating": "A++"
          }
        },
        {
          "term": {
            "type": "Fridge Freezer"
          }
        }
      ],
      "filter": [
        {
          "range": {
            "price": {
              "gte": 500,
              "lte": 1000
            }
          }
        }
      ]
    }
  }
}
```

## Named queries

当我们进行复合查询时，不知道那些条件命中了，如果想知道，可以在查询中标记_name，返回结果如果包含则说明命中了

```json
#Named queries for "All clauses combined" query
GET products/_search
{
  "_source": ["product", "brand"], 
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "brand": {
              "query": "LG",
              "_name": "must_match_brand_query"
            }
          }
        }
      ],
      "must_not": [
        {
          "match": {
            "colour.keyword": {
              "query":"black",
              "_name":"must_not_colour_query"
            }
          }
        }
      ],
      "should": [
        {
          "term": {
            "type.keyword": {
              "value": "Frost Free Fridge Freezer",
              "_name":"should_term_type_query"
            }
          }
        },
        {
          "match": {
            "energy_rating": {
              "query": "A++",
              "_name":"should_match_energy_rating_query"
            }
          }
        }
      ],
      "filter": [
        {
          "range": {
            "price": {
              "gte": 500,
              "lte": 1000,
              "_name":"filter_range_price_query"
            }
          }
        }
      ]
    }
  }
}

// output
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
      "value" : 2,
      "relation" : "eq"
    },
    "max_score" : 5.869732,
    "hits" : [
      {
        "_index" : "products",
        "_type" : "_doc",
        "_id" : "20",
        "_score" : 5.869732,
        "_ignored" : [
          "overview.keyword"
        ],
        "_source" : {
          "product" : "Fridge",
          "brand" : "LG"
        },
        "matched_queries" : [
          "should_match_energy_rating_query",
          "filter_range_price_query",
          "must_match_brand_query",
          "should_term_type_query"
        ]
      }
    ]
  }
}
```

# Constant scores

constant_score表示就是相关性评分时指定字段的score为常量。

可以针对性的对某个字段进行评分提升，filter本身不参与评分（评分为0），设置boost可以提高score。

```json
# Bool query with a constant score
GET products/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "product": "TV"
          }
        },
        {
          "constant_score": {
            "filter": {
              "term": {
                "colour": "black"
              }
            },
            "boost": 3.5
          }
        }
      ]
    }
  }
}
```

# The boosting query

我们都是有偏好的，有时我们想要tv但是不希望价格太贵，那搜索的时候就可以使用boosting。

```json
# Boosting query
GET products/_search
{
  "size": 40, 
  "_source": ["product", "price","colour"], 
  "query": {
    "boosting": {
      "positive": {
        "term": {
          "product":"tv"
        }
      },
      "negative": {
        "range": {
          "price": {
            "gte": 2500
          }
        }
      },
      "negative_boost": 0.5
    }
  }
}

# Boosting query combined with bool queries
GET products/_search
{
  "size": 40, 
  "_source": ["product", "price","colour","brand"], 
  "query": {
    "boosting": {
      "positive": {
        "bool": {
          "must": [
            {
              "match": {
                "product": "TV"
              }
            }
          ]
        }
      },
      "negative": {
        "bool": {
          "must": [
            {
              "match": {
                "brand": "Sony"
              }
            }
          ]
        }
      },
      "negative_boost": 0.5
    }
  }
}
```

# The disjunction max(dis_max) query

dis_max不是很理解，不纠结了

```json
# Multi_match
GET products/_search
{
  "query": {
    "multi_match": {
      "query": "smart",
      "fields": ["type","overview"]
    }
  }, "highlight": {
    "fields": {
      "type": {},
      "overview": {}
    }
  }
}

GET products/_search
{
  "_source": ["type","overview"], 
  "query": {
    "dis_max": {
      "queries": [{
        "match": {
          "type": "smart tv"
        }
      },
      {
        "match": {
          "overview": "smart tv"
        }
      },
      {
        "match": {
          "product": "smart tv"
        }
      }],"tie_breaker": 0.5  
    }
  }
}
```

# The function_score query

function_score的使用场景是使用用户自定义的评分算法来生成新的排序。

前面说了按字段排序不会进行评分，但不评分就没有相关性，所以function_score是为了解决这一问题。

```json
#Function score - wraping the term query - score is 1.6376086

GET products/_search
{
  "query": {
    "function_score": {
      "query": {
        "term": {
          "product": "tv"
        }
      }
    }
  }
}

#Function score - random score
GET products/_search
{
  "query": {
    "function_score": {
      "query": {
        "term": {
          "product": "tv"
        }
       },
       "random_score": {}
    }
  }
}

#Field value factor
GET products/_search
{
  "query": {
    "function_score": {
      "query": {
        "term": {
          "product": "tv"
        }
      },
      "field_value_factor": {
        "field": "user_ratings",
        "factor": 2,
        "modifier": "square"
      }
    }
  }
}
```
