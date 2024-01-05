## Testing analyzers

如果要看分词的结果，使用`_analyze`。

```json
GET _analyze
{
  "text": "James Bond 007"
}

// output:
{
  "tokens" : [
    {
      "token" : "james",
      "start_offset" : 0,
      "end_offset" : 5,
      "type" : "<ALPHANUM>",
      "position" : 0
    },
    {
      "token" : "bond",
      "start_offset" : 6,
      "end_offset" : 10,
      "type" : "<ALPHANUM>",
      "position" : 1
    },
    {
      "token" : "007",
      "start_offset" : 11,
      "end_offset" : 14,
      "type" : "<NUM>",
      "position" : 2
    }
  ]
}
```

ALPHANUM for a string, NUM for a numeric token.

不传analyzer默认standard analyzer，可指定analyzer

```json
GET _analyze
{
  "text": "James Bond 007",
  "analyzer": "simple"
}

output:
{
  "tokens" : [
    {
      "token" : "james",
      "start_offset" : 0,
      "end_offset" : 5,
      "type" : "word",
      "position" : 0
    },
    {
      "token" : "bond",
      "start_offset" : 6,
      "end_offset" : 10,
      "type" : "word",
      "position" : 1
    }
  ]
}
```

`_analyze`还可以指定tokenizer和filter后使用：

```json
GET _analyze
{
  "text": "/Volumes/FILES/Dev",
  "tokenizer": "path_hierarchy",
  "filter": [
    "uppercase"
  ]
}
```

# Built-in analyzers

八种analyzer：

1. standard：default analyzer
2. simple
3. stop
4. whitespace
5. keyword
6. language
7. pattern
8. fingerprint

## standard analyzer

```json
GET _analyze
{
  "analyzer": "standard",
  "text": "Hot cup of ☕ and a 🍿is a Weird Combo :(!!"
}
```

stop filter默认不开启，可配置开启

```json
#Configuring analyzers

PUT my_index_with_stopwords
{
  "settings": {
    "analysis": {
      "analyzer": {
        "standard_with_stopwords":{ # names the analyzer
          "type":"standard",
          "stopwords":"_english_"  # enables the English stop words filter
        }
      }
    }
  }
}
```



```json
POST my_index_with_stopwords/_analyze
{
  "text": ["Hot cup of ☕ and a 🍿is a Weird Combo :(!!"],
  "analyzer": "standard_with_stopwords"
}
```

of, a, is 这些词将不再出现在分词中。



也可以将stop words放在一个文件中，这个文件的位置是放置在`$ELASTICSEARCH_HOME/config` 中的。

```json
# Index with custom swear words 
PUT index_with_swear_stopwords
{
  "settings": {
    "analysis": {
      "analyzer": {
        "swearwords_analyzer":{
          "type":"standard",
          "stopwords_path":"swearwords.txt"
        }
      }
    }
  }
}

POST index_with_swear_stopwords/_analyze
{
  "text": ["Damn, that sucks!"],
  "analyzer": "swearwords_analyzer"
}
```



### Configuring the token length

```json
## Setting token length
DELETE my_index_with_max_token_length
PUT my_index_with_max_token_length
{
  "settings": {
    "analysis": {
      "analyzer": {
        "standard_max_token_length":{
          "type":"standard",
          "max_token_length":7
        }
      }
    }
  }
}
```

比如`Elasticsearch`就会被拆分为`Elastic, search`。
