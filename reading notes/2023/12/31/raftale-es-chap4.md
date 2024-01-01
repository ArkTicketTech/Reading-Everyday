## the date data type

## numeric data types

## the boolean data type

## the range data type

range data type代表了一个范围。有很多类型的range：

```
date_range, integer_range, float_range, ip_range, ...
PUT trainings
{
  "mappings":{
    "properties":{
      "name":{
        "type":"text"
      },
      "training_dates":{
        "type":"date_range"
      }
    }
  }
}

PUT trainings/_doc/1 
{
  "name":"Functional Programming in Java",
  "training_dates":{ 
    "gte":"2021-08-07",
    "lte":"2021-08-10"
  }
}
PUT trainings/_doc/2 
{
  "name":"Programming Kotlin",
  "training_dates":{
    "gte":"2021-08-09",
    "lte":"2021-08-12"
  }
}
PUT trainings/_doc/3
{
  "name":"Reactive Programming",
  "training_dates":{
    "gte":"2021-08-17",
    "lte":"2021-08-20"
  }
}
```





```json
GET trainings/_search
{
  "query": {
    "range": { 
      "training_dates": {
        "gte": "2021-08-10",
        "lte": "2021-08-12"
      }
    }
  }
}
```

## the ip data type

# Advanced data Types

## the geopoint data type

## the object data type

`attachments`就是object type，object type不会显式定义，其实就是json中的对象。

```json
PUT emails
{
  "mappings": {
    "properties": { 
      "to":{
        "type": "text"
      },
      "subject":{
        "type": "text"
      },
      "attachments":{ 
        "properties": {
          "filename":{
            "type":"text"
          },
          "filetype":{
            "type":"text"
          }
        }
      }
    }
  }
}
PUT emails/_doc/1
{
  "to:":"johndoe@johndoe.com",
  "subject":"Testing Object Type",
  "attachments":{
    "filename":"file1.txt",
    "filetype":"confidential"
  }
}

GET emails/_search
{
  "query": {
    "match": {
      "attachments.filename": "file1.txt"
    }
  }
}
```



```json
PUT emails/_doc/2
{
  "to:":"mrs.doe@johndoe.com",
  "subject":"Multi attachments test",
  "attachments":[{
    "filename":"file2.txt",
    "filetype":"confidential"
  },{
    "filename":"file3.txt",
    "filetype":"private"
  }]
}


GET emails/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "attachments.filename": "file1.txt"
          }
        },
        {
          "match": {
            "attachments.filetype": "confidential"
          }
        }
      ]
    }
  }
}
```

上面的查询本意是返回文档1，实际上返回了文档1和文档2，因为文档1默认的filetype默认设置为`confidential`了，而object type是将内部对象`flattened`化存储：

```json
{
  "attachments.filename" :["file1.txt"]
  "attachments.filetype":["confidential"]
}
{
  ...
  "attachments.filename" :["file2.txt","file3.txt"]
  "attachments.filetype":["private","confidential"]
}
```

所以object对象会丢失filetype和filename的关系。



## the nested data type

nested data type就是用来解决上面的object data type面临的问题。

```json
PUT emails_nested
{
  "mappings": {
    "properties": {
      "attachments": {
        "type": "nested",
        "properties": {
          "filename": {
            "type": "keyword"
          },
          "filetype": {
            "type": "text" 
          }
        }
      }
    }
  }
}

PUT emails_nested/_doc/1
{
  "attachments" : [
    {
      "filename" : "file1.txt",
      "filetype" :  "confidential"
    },
    {
      "filename" : "file2.txt",
      "filetype" :  "private"
    }
  ]
}

GET emails_nested/_search
{
  "query": {
    "nested": {
      "path": "attachments",
      "query": {
        "bool": {
          "must": [
            {
              "match": {
                "attachments.filename": "file1.txt"
              }
            },
            {
              "match": {
                "attachments.filetype": "confidential"
              }
            }
          ]
        }
      }
    }
  }
}
```

当nested data type使用时，其实就是隐式的创建了一个新的文档。

Elasticsearch没有array type，如果插入数据时value为["john", "tom"]，那么dynamic mapping会根据array的第一个元素进行类型推断，这里就是string，一个array中不能包含不同类型的数据，否则会报错。



## flattened data type

flattened data type意味着该对象下所有值都将被认为是keyword，不被认为text就不需要进行文本分析过程，就节省了开销。

如果我们的场景是不需要进行文本分析，比如直播足球比赛中的实时评论，那么使用该类型可以提升性能。

```json
PUT consultations
{
  "mappings": {
    "properties": {
      "patient_name":{
        "type": "text"
      },
      "doctor_notes":{ 
        "type": "flattened"
      }
    }
  }
}

PUT consultations/_doc/1
{
  "patient_name":"John Doe",
  "doctor_notes":{
    "temperature":103,
    "symptoms":["chills","fever","headache"],
    "history":"none",
    "medication":["Antibiotics","Paracetamol"]
  }
}
```

那么`103, chills, fever, headache, none, Antibiotics, Paracetamol`都将被认为是keyword，搜索

```json
GET consultations/_search
{
  "query": {
    "match": {
      "doctor_notes": "Paracetamol"
    }
  }
}


# An advanced query on a flattened data type:

GET consultations/_search
{
  "query": {
    "bool": {
      "must": [
        {
          "match": {
            "doctor_notes": "headache"
          }
        },
        {
          "match": {
            "doctor_notes": "Antibiotics"
          }
        }
      ],
      "must_not": [
        {
          "term": {
            "doctor_notes": {
              "value": "diabetics"
            }
          }
        }
      ]
    }
  }
}
```

flattened data type非常适合 预期我们有很多字段需要临时添加，并且事先为所有字段定义mapping不可行时。

## the Join data type

Join实际上就是将文档关联起来，比如写入一个医生的文档，再写入他的病人的文档，文档之间用join关联起来，一般为parent-child关系。

```json
## Join data type
# Mapping of the doctors schema definition

PUT doctors
{
  "mappings": {
    "properties": {
      "relationship":{
        "type": "join",
        "relations":{
          "doctor":"patient" 
        }
      }
    }
  }
}

# Indexing a doctor document - note the relationship attribute:

PUT doctors/_doc/1
{
  "name":"Dr Mary Montgomery",
  "relationship":{
    "name":"doctor" 
  }
}

# Creating two patients for our doctor:

PUT doctors/_doc/2?routing=mary
{
  "name":"John Doe",
  "relationship":{
    "name":"patient",
    "parent":1
  }
}

PUT doctors/_doc/3?routing=mary
{
  "name":"Mrs Doe",
  "relationship":{
    "name":"patient",
    "parent":1
  }
}



#Fetching the patients of Dr Montgomery

GET doctors/_search
{
  "query": {
    "parent_id":{
      "type":"patient",
      "id":1
    }
  }
}
```

parent和child的数据应该在一个分片中，来避免多分片开销。所以`?routing=mary`的作用就是划分到同一个分片中，这个在第5章讲。

join有性能开销，谨慎使用。

## search as you type data type

有些场景：

1. search as you tyep
2. typeahead
3. autocomplete
4. suggestions

需要预判用户的输入，如果我们已经收集了相关的数据，那么可以用es来实现。es提供了`search_as_you_type`类型来支持这一特性。

```json
#Mapping schema for technical books with the title as search_as_you_type type
PUT tech_books
{
  "mappings": {
    "properties": {
      "title": {
        "type": "search_as_you_type"
      }
    }
  }
}

#Indexing a few books:

PUT tech_books4/_doc/1
{
  "title":"Elasticsearch in Action"
}

PUT tech_books4/_doc/2
{
  "title":"Elasticsearch for Java Developers"
}

PUT tech_books4/_doc/3
{
  "title":"Elastic Stack in Action"
}

# Searching in a search_as_you_type field and its subfields

GET tech_books4/_search
{
  "query": {
    "multi_match": {
      "query": "in",
      "type": "bool_prefix", 
      "fields": ["title"]
    }
  }
}


# _index_prefix produces edge n-grams - [e, el, ela, elas, elast, elasti, elastic]
GET tech_books4/_analyze
{
  "field": "title._index_prefix",
  "text": ["Elastic"]
}

# 2gram produces two tokens - [elaticsearch, in],[in, action]
GET tech_books4/_analyze
{
  "field": "title._2gram",
  "text": ["Elasticsearch in Action"]
}

# 3gram produces three tokens - [elaticsearch, for, java],[for, java, developers]
GET tech_books4/_analyze
{
  "field": "title._3gram",
  "text": ["Elasticsearch for Java Developers"]
}
```

查找原理就是利用到了ngrams，实际上就是分词。第7章text analysis会讲。

1. ngrams: 以action为例

1. 3-ngrams：["act", "cti","tio","ion"]
2. 2-ngrams：["ac", "ct","ti","io","on"]

1. Edge ngrams：以action为例

1. ["a", "ac","act","acti","actio","action"]

1. Shingles：以`Elasticsearch in Action`

1.  ["Elasticsearch", "Elasticsearch in", "Elasticsearch in Action", "in", "in Action", "Action"]
2. 上面的`_index_prefix`就是Shingles

# Multiple data types

es可以为一个字段定义多个data type。

```json
PUT emails
{
  "mappings": {
    "properties": {
      "subject":{
        "type": "text",
        "fields": {
          "kw":{ "type":"keyword" },
          "comp":{ "type":"completion" }
        }
      }
    }
  }
}

GET emails_multi_field/_mapping

{
  "emails_multi_field" : {
    "mappings" : {
      "properties" : {
        "subject" : {
          "type" : "text",
          "fields" : {
            "comp" : {
              "type" : "completion",
              "analyzer" : "simple",
              "preserve_separators" : true,
              "preserve_position_increments" : true,
              "max_input_length" : 50
            },
            "kw" : {
              "type" : "keyword"
            }
          }
        }
      }
    }
  }
}
 
```

比如上面给subject字段定义了`text, keyword, completion`三种数据类型。
