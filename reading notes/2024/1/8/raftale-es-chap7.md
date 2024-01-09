# Custom analyzers

自定义analyzer，type须为`custom`，然后定义`char_filter, tokenizer, filter`。

```json
## Custom Analyzer
PUT index_with_custom_analyzer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "custom_analyzer":{
          "type":"custom",
          "char_filter":["html_strip"],
          "tokenizer":"standard",
          "filter":["uppercase"]
        }
      }
    }
  }
}
```

## Advanced customization

可以自定义`char_filter, filter`。

```json
DELETE index_with_parse_greek_letters_custom_analyzer
PUT index_with_parse_greek_letters_custom_analyzer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "greek_letter_custom_analyzer":{
          "type":"custom",
          "char_filter":["greek_symbol_mapper"],
          "tokenizer":"standard",
          "filter":["lowercase", "greek_keep_words"]
        }
      },
      "char_filter": {
        "greek_symbol_mapper":{
          "type":"mapping",
          "mappings":[
            "α => alpha",
            "β => Beta",
            "γ => Gamma"
          ]
        }
      },
      "filter": {
        "greek_keep_words":{
          "type":"keep",
          "keep_words":["alpha", "beta", "gamma"]
        }
      }
    }
  }
}
  
  
  
POST index_with_custom_greek_analyzer/_analyze
{
  "text": "α and β are roots of a quadratic equation",
  "analyzer": "custom_analyzer"
}
// output: ["alpha", "beta", "gamma"]
```



# Specifying analyzers

## Analysers for indexing

analyzer不只是能在index-level，而且能精确到某个字段。

```json
# Setting analyzers at field level during index creation
PUT authors_with_field_level_analyzers
{
  "mappings": {
    "properties": {
      "name":{
        "type": "text" 
      },
      "about":{
        "type": "text",
        "analyzer": "english" 
      },
      "description":{
        "type": "text",
        "fields": {
          "my":{
            "type": "text",
            "analyzer": "fingerprint" 
          }
        }
      }
    }
  }
}
```



analyzer在index-level下的，替换掉默认的standard analyzer。

```json
# Creating an index with a default analyzer
PUT authors_with_default_analyzer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "default":{ 
          "type":"keyword"
        }
      }
    }
  }
}
```



## Analysers for searching

搜索时也可以指定analyzer

```json
# Setting the analyzer alongside a search query
GET authors_index_for_search_analyzer/_search
{
  "query": {
    "match": { 
      "author_name": {
        "query": "M Konda",
        "analyzer": "simple" 
      }
    }
  }
}
# Setting a search analyzer at a field level
PUT authors_index_with_both_analyzers_field_level
{
  "mappings": {
    "properties": {
      "author_name":{
        "type": "text",
        "analyzer": "stop",
        "search_analyzer": "simple"
      }
    }
  }
}
```

# Character filters

character filter帮助我们预先去除对搜索没有益处的字符，比如标点符号和特殊字符。

1. 去除不想要的字符
2. 增加或者替换字符：比如boolean变量，0和1分别代表false和true

## HTML strip filter

去除不需要的html tag

```json
POST _analyze
{
  "text":"<h1>Where is my cheese?</h1>",
  "tokenizer": "standard", 
  "char_filter": ["html_strip"]
}
```

但如果我们想保留一些html tag也是可以的

```json
PUT index_with_html_strip_filter
{
  "settings": {
    "analysis": {
      "analyzer": {
        "custom_html_strip_filter_analyzer":{
          "tokenizer":"keyword",
          "char_filter":["my_html_strip_filter"]
        }
      },
      "char_filter": {
        "my_html_strip_filter":{
          "type":"html_strip",
          "escaped_tags":["h1"]
        }
      }
    }
  }
}


POST index_with_html_strip_filter/_analyze
{
  "text": "<h1>Hello,</h1> <h2>World!</h2>",
  "analyzer": "custom_html_strip_filter_analyzer"
}
```

## The mapping character filter

```json
# Mapping Char Filter
POST _analyze
{
  "text": "I am from UK",
  "char_filter": [
    {
      "type": "mapping",
      "mappings": [
        "UK => United Kingdom"
      ]
    }
  ]
}

// output: 
{
  "tokens" : [
    {
      "token" : "I am from United Kingdom",
      "start_offset" : 0,
      "end_offset" : 12,
      "type" : "word",
      "position" : 0
    }
  ]
}
```



```json
DELETE index_with_mapping_char_filter

# Custom analyzer wiht a mapping characte filter
PUT index_with_mapping_char_filter
{
  "settings": {
    "analysis": {
      "analyzer": {
        "my_social_abbreviations_analyzer": {
          "tokenizer": "keyword",
          "char_filter": [
            "my_social_abbreviations"
          ]
        }
      },
      "char_filter": {
        "my_social_abbreviations": {
          "type": "mapping",
          "mappings": [
            "LOL => laughing out loud",
            "BRB => be right back",
            "OMG => oh my god"
          ]
        }
      }
    }
  }
}

POST index_with_mapping_char_filter/_analyze
{
  "text": "LOL",
  "analyzer": "my_social_abbreviations_analyzer"
}
```

事实上，mapping还可以通过file实现。

## The pattern_replace character filter

正则替换

```json
# Pattern replce charcter filter
DELETE index_with_pattern_replace_filter
PUT index_with_pattern_replace_filter
{
  "settings": {
    "analysis": {
      "analyzer": {
        "my_pattern_replace_analyzer":{
          "tokenizer":"keyword",
          "char_filter":["pattern_replace_filter"]
        }
      },
      "char_filter": {
        "pattern_replace_filter":{
          "type":"pattern_replace",
          "pattern":"_",
          "replacement":"-"
        }
      }
    }
  }
}

POST index_with_pattern_replace_filter/_analyze
{
  "text": "Apple_Boy_Cat",
  "analyzer": "my_pattern_replace_analyzer"
}
```

# Tokenizers

## The Standard tokenizer

standard tokenizer基于空格和标点符号将文本拆分为token。

stand analyzer唯一可配置的 属性是`max_token_length`，默认为255，表示一个token最大的尺寸，超过就会进行拆分。

```json
# Cutom stadard tokenizer
PUT index_with_custom_standard_tokenizer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "custom_token_length_analyzer": {
          "tokenizer": "custom_token_length_tokenizer"
        }
      },
      "tokenizer": {
        "custom_token_length_tokenizer": {
          "type": "standard",
          "max_token_length": 2
        }
      }
    }
  }
}

POST index_with_custom_standard_tokenizer/_analyze {
  "text": "Bond",
  "analyzer": "custom_token_length_analyzer"
}
```

## N-gram and edge_ngram tokenizers

n-gram相当于是用了一个长度为n的滑动窗口去滑动这个文本。

edge_ngram相当于是固定左边界，然后依次向右滑动的滑动窗口。


### the ngram tokenizer

定制化ngram tokenzier，可以配置最小和最大的窗口尺寸

```json
# Custom n-gram tokenizer
DELETE index_with_ngram_tokenizer
PUT index_with_ngram_tokenizer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "ngram_analyzer":{
          "tokenizer":"ngram_tokenizer"
        }
      },
      "tokenizer": {
        "ngram_tokenizer":{
          "type":"ngram",
          "min_gram":2,
          "max_gram":3,
          "token_chars":[
            "letter"
          ]
        }
      }
    }
  }
}

POST index_with_ngram_tokenizer/_analyze
{
  "text": "bond",
  "analyzer": "ngram_analyzer"
}
```

### edge ngram

`edge_ngram`默认最小长度为1，最大长度为2.

```json
#Edge-ngram

POST _analyze
{
  "text":"hello",
  "tokenizer": "edge_ngram"
}
// output : [h, he]
```

定制化：

```json
PUT index_with_edge_ngram
{
  "settings": {
    "analysis": {
      "analyzer": {
        "edge_ngram_analyzer":{
          "tokenizer":"my_edge_ngram_tokenizer"
        }
      },
      "tokenizer": {
        "my_edge_ngram_tokenizer":{
          "type":"edge_ngram",
          "min_gram":2,
          "max_gram":6,
          "token_chars":["letter","digit"]
        }
      }
    }
  }
}

POST index_with_edge_ngram/_analyze
{
  "text": "bond",
  "analyzer": "edge_ngram_analyzer"
}
```

token_chars是指character的类型，遇到不在token_chars的滑动就会停止。



## Other tokenizers

其他的就不介绍了，要用的时候再看

# Token filters

token filters主要是丰富token的含义，比如同义词、大小写、词根、移除标点符号等。

Elasticsearch提供了50个token filters。

```json
GET _analyze
{
  "tokenizer" : "standard",
  "filter" : ["uppercase","reverse"],
  "text" : "bond"
}

// ["DNOB]
```

我们也可以将token filters添加到自定义的analyzer中。

```json
PUT index_with_token_filters
{
  "settings": {
    "analysis": {
      "analyzer": {
        "token_filter_analyzer": {
          "tokenizer": "standard",
          "filter": [
            "uppercase",
            "reverse"
          ]
        }
      }
    }
  }
}

GET index_with_token_filters/_analyze
{
  "text":"bond",
  "analyzer": "token_filter_analyzer"
}
// [DONB]
```

## The stemmer filter

将单词转化为词根

```json
POST _analyze
{
  "tokenizer": "standard",
  "filter": ["stemmer"],
  "text": "barking is my life"
}
```

[bark, is, my, life]

## The shingle filter

之前讲到的ngram和edge gram是作用在character上的，而shingle是作用在word上的。

```json
POST _analyze
{
  "tokenizer": "standard",
  "filter": ["shingle"],
  "text": "java python go"
}
// output: [java, java python, python, python go, go]
```



当然，如果要改变滑动窗口的尺寸，也可以进行定制：

```json
DELETE index_with_shingle
PUT index_with_shingle
{
  "settings": {
    "analysis": {
      "analyzer": {
        "shingles_analyzer":{
          "tokenizer":"standard",
          "filter":["shingles_filter"]
        }
      },
      "filter": {
        "shingles_filter":{
          "type":"shingle",
          "min_shingle_size":2,
          "max_shingle_size":3,
          "output_unigrams":false
        }
      }
    }
  }
}


POST index_with_shingle/_analyze
{
  "text": "java python go",
  "analyzer": "shingles_analyzer"
}
```

输出: ["java python", "java python go", "python go"]

unigram是指one-word shingles，如java,python, go。因为`output_unigrams`为false，这三个将被移除。

## The synonym filter

同义词替换，当然数据源也可以来自于文本文件

```json
# Index with a synonyms filter
PUT index_with_synonyms
{
  "settings": {
    "analysis": {
      "filter": {
        "synonyms_filter":{
          "type":"synonym",
         "synonyms":[ "soccer => football"]
        }
      }
    }
  }
}

# Test the index - the output should be "What" "Football"
POST index_with_synonyms/_analyze
{
  "text": "What's soccer?",
  "tokenizer": "standard", 
  "filter": ["synonyms_filter"]
}

PUT index_with_synonyms_analyzer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "synonyms_analyzer":{
          "type":"custom",
          "tokenizer":"standard",
          "filter":["synonyms_filter"]
        }
      }
      ,"filter": {
        "synonyms_filter":{
          "type":"synonym",
          "synonyms":["lol => Laughing out loud"]
        }
      }
    }
  }
}
```
