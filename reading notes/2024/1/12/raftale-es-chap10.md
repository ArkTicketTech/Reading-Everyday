The chapter covers

1. Overview of full-text queries
2. Working through match queries
3. Looking at match phrases, multi-match, and other queries
4. Looking at query strings and simple query string queries

# Overview

相关性评判的两个重要指标：precision和recall

## Precision

precision是返回的文档中具有相关性文档的占比，查询时返回的结果并不是所有都与查询有关，可能包含一些不相关的文档。




## Recall

召回率是指本来有10个相关性的数据，但只返回了6个，那么召回率就是16%。召回率反映了在整体数据中有多少相关性样本被找到了，precision反映的是返回数据中有多少是相关的。


precison和recall是相互制约的，precison越高，recall越低。

# Sample data

https://github.com/madhusudhankonda/elasticsearch-in-action/blob/f334df2dd5f10acc15bfc745f580ec9be0b129d2/datasets/books.txt

# The match_all query

## Building the match_all query

返回所有数据，即召回率100%。

```json
# Match queries

##Match all query
GET books/_search
{
  "query": {
    "match_all": {}
  }
}
```

## Short form of a match_all query

```
GET books/_search
```

# The match_none query

```json
GET books/_search
{
  "query": {
    "match_none": {

    }
  }
}

{
  "query": {
    "bool": {
      "must": [{
        "match_none": {}
      }] 
    }
  } 
}
```

match_none就是不返回任何数据，看起来是没啥用的，但在有些场景有点用，比如数据库由于维护或者升级被锁定的情况下，你不希望应用系统抛出错误，而是不返回任何结果，那么就可以在bool查询中插入match_none条件。

# The match query



## Format of a match query

简化版和完整版：

```json
GET books/_search
{
  "query": {
    "match": {
      "FIELD": "<SEARCH TEXT>"
    }
  } 
}

GET books/_search
{
  "query": {
    "match": {
      "FIELD": {
        "query": "<SEARCH TEXT>",
        "<parameter>": "<MY_PARAM>"
      } 
    }
  } 
}
```

## Analyzing match queries

text会进行文本分析

## Searching for multiple words

```json
# Match query searching Java titles
GET books/_search
{
  "query": {
    "match": {
      "title": "Java"
    }
  }
}

# Match query searching for a set of words (like a sentence)
GET books/_search
{
  "query": {
    "match": {
      "title": {
        "query": "Java Complete Guide"
      }
    }
  },
  "highlight": {
    "fields": {
      "title": {}
    }
  }
}

# Same as above query but specifying OR operator explicitly
GET books/_search
{
  "query": {
    "match": {
      "title": {
        "query": "Java Complete Guide",
        "operator": "OR"
      }
    }
  }
}

# Specifying AND operator explicitly
GET books/_search
{
  "query": {
    "match": {
      "title": {
        "query": "Java Complete Guide",
        "operator": "AND"
      }
    }
  }
}
```

## Matching at least a few words

```json
# Matching at least two words 
GET books/_search
{
  "query": {
    "match": {
      "title": {
        "query": "Java Complete Guide",
        "operator": "OR",
        "minimum_should_match": 2
      }
    }
  }
}
```

## Fixing typos using the fuzziness keyword

```json
# Matching on an incorrect title (spelling mistake) - with fuzziness- postive results
GET books/_search
{
  "query": {
    "match": {
      "title": {
        "query": "Kava",
        "fuzziness": 1
      }
    }
  }
}
```

# The match_phrase query

有时我们希望查询 精确包含某句话的结果，就可以用`match_phrase`：

```json
# Match phrase query
GET books/_search
{
  "query": {
    "match_phrase": {
      "synopsis": "book for every Java programmer"
    }
  }
}


# Match phrase query with slop - missing one word
GET books/_search
{
  "query": {
    "match_phrase": {
      "synopsis": {
       "query": "book every Java programmer",
       "slop": 1
      }
    }
  }
}

# Match phrase query with jumbled up words will not work - even with slop
GET books/_search
{
  "query": {
    "match_phrase": {
      "synopsis": {
       "query": "for every Java programmer book",
       "slop": 1
      }
    }
  }
}
```

如果允许不那么精确，比如少匹配一个词（只能少，不能错），那就可以用slop

# The match_phrase_prefix query

phrase的最后一个词作为前缀，其余跟match_phrase_prefix是一样的

```json
# Match phrase prefix 
GET books/_search
{
  "query": {
    "match_phrase_prefix": {
      "tags": {
        "query": "concepts and found"
      }
    }
  },
  "highlight": {
    "fields": {
      "tags": {}
    }
  }
}
# Match phrase prefix with slop - omitting "and"
GET books/_search
{
  "query": {
    "match_phrase_prefix": {
      "tags": {
        "query": "concepts found",
        "slop":1
      }
    }
  }
}
```

# The multi_match query

```json
# Multi-match query
GET books/_search
{
  "_source": false, 
  "query": {
    "multi_match": {
      "query": "Java",
      "fields": [
        "title",
        "synopsis",
        "tags"
      ]
    }
  },
  "highlight": {
    "fields": {
      "title": {},
      "tags": {}
    }
  }
}
```

## Best fields

在多个字段中搜素`java collections`进行匹配时，两个单词都在同一个字段匹配比分开在两个字段匹配具有更高的相关性。

匹配了所有单词的字段称为best fields，match默认的匹配就是best fields。

```json
# Best fields
GET books/_search
{
  "query": {
    "multi_match": {
      "query": "Design Patterns",
      "type": "best_fields",
      "fields": [
        "title",
        "synopsis"
      ]
    }
  },
  "highlight": {
    "fields": {
      "tags": {},
      "title": {}
    }
  }
}

# Best fields - with a tie breaker
GET books/_search
{
  "_source": false,
  "query": {
    "multi_match": {
      "query": "Design Patterns",
      "type": "best_fields",
      "fields": [
        "title",
        "synopsis"
      ],
      "tie_breaker": 0.5
    }
  },
  "highlight": {
    "fields": {
      "tags": {},
      "title": {}
    }
  }
}
```

type有很多，还有cross_fields, most_fields, phrase, phrase_prefixx等。

## The dis_max query

multi_match可以在多个字段中搜素标准，为了在后台执行这种查询类型，Elasticsearch会使用一个叫做disjunction max query的查询（dis_max）。

dis_max会将每个字段分割为单独的匹配查询。

```json
# dis_max query
GET books/_search
{
  "query": {
    "dis_max": {
      "queries": [
        {"match": {"title": "Design Patterns"}},
        {"match": {"synopsis": "Design Patterns"}}]
    }
  }
}
```

上面的执行结果与multi_match实际上是一样的。

dis_max实际上是复合查询，下一章会讲。

## Tiebreakers

相关性分数是基于单字段分数的，tie_breaker的公式：

```
 Overall score = score of the best match field + score of the other matching fields * tie_breaker
# dis_max query
GET books/_search
{
  "_source": false, 
  "query": {
    "dis_max": {
      "queries": [
        {"match": {"title": "Design Patterns"}},
        {"match": {"synopsis": "Design Patterns"}}],
        "tie_breaker": 0.5
    }
  },
  "highlight": {
    "fields": {
      "title": {},
      "synopsis": {},
      "tags": {}
    }
  }
}
```

猜测其目的是为了提高其他匹配字段的权重，因为有可能两个搜索best_field的得分差的不大，但是其他字段差别很大。
