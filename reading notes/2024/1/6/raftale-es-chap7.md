## Simple analyzer

只要遇到不是字母的字符，simple analyzer就会将text进行分解，并且所有的词语都是小写。

```json
POST _analyze
{
  "text": ["Lukša's K8s in Action"],
  "analyzer": "simple"
}

# output: ["lukša","s","k","s","in","action"]
```


## Whitespace analyzer

按空格拆分


## Keyword analyzer

keyword analyzer不会tokenize the text，也没有filter分析，它只是把文本作为一个keyword存储，意味着搜索的时候需要 全部匹配才能查询。


noop tokenizer：no-operation tokenizer

```json
POST _analyze
{
  "text": "Elasticsearch in Action",
  "analyzer": "keyword"
}

# output
{
  "tokens" : [
    {
      "token" : "Elasticsearch in Action",
      "start_offset" : 0,
      "end_offset" : 23,
      "type" : "word",
      "position" : 0
    }
  ]
}
```



## Fingerprint analyzer

指纹分析器，可以理解为类似的文本经过一系列处理后，最后的文本可能是相同的。

由一个standard tokenizer和四个filter组成：

1. lowercase
2. asciifolding：将不在前127个ASCII的字母、数字、字符转化为等效的字符
3. stop：默认不开启
4. fingerprint：删除重复数据，并排序token，



```json
GET /_analyze
{
  "tokenizer": "standard",
  "filter": [
    "asciifolding"
  ],
  "text": "açaí à la carte"
}

// output: ["acai","a","la","carte"]
```

```json
POST _analyze
{
  "text": "A dosa is a thin pancake or crepe originating from South India. ➥ It is made from a fermented batter consisting of lentils and rice.",
  "analyzer": "fingerprint"
}
# output
{
  "tokens" : [
    {
      "token" : "a and batter consisting crepe dosa fermented from india is it lentils made of or originating pancake rice south thin",
      "start_offset" : 0,
      "end_offset" : 132,
      "type" : "fingerprint",
      "position" : 0
    }
  ]
}
```

## Pattern analyzer

实际上就是用正则去做匹配

```json
# Dash pattern
PUT index_with_dash_pattern_analyzer
{
  "settings": {
    "analysis": {
      "analyzer": {
        "pattern_analyzer": {
          "type":      "pattern",
          "pattern":   "[-]", 
          "lowercase": true
        }
      }
    }
  }
}

POST index_with_dash_pattern_analyzer/_analyze
{
  "text": "1234-5678-9000-0000",
  "analyzer": "pattern_analyzer"
}
```
## Language analyzers

按照各国的语言进行分析为单词，需要的时候再详细了解。
