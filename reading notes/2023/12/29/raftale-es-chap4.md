1. Filed data types
2. Implicit and explicit mapping
3. Core data types
4. Advanced data types
5. APIs to create/access the mappings

# Overview

当我们存储数据时，并没有标记字段的类型，es会自动推断字段类型。

不同于关系型数据库，需要预先定义schema，es是schema-free的。

schema也是可以显示定义的，创建schema 定义的过程被称为mapping。



mapping其实就是在定义文档的字段和类型，准确无误的mapping能使elasticsearch更精确的工作。






```json
PUT movies/_doc/1
{
  "title":"Godfather",
  "rating":4.9,
  "release_year":"1972/08/01"
}
```



```
GET movies/_mapping
{
  "movies" : {
    "mappings" : {
      "properties" : {
        "rating" : {
          "type" : "float"
        },
        "release_year" : {
          "type" : "date",
          "format" : "yyyy/MM/dd HH:mm:ss||yyyy/MM/dd||epoch_millis"
        },
        "title" : {
          "type" : "text",
          "fields" : {
            "keyword" : {
              "type" : "keyword",
              "ignore_above" : 256
            }
          }
        }
      }
    }
  }
}
```



# Dynamic Mapping

不是通过显式定义mapping的，直接插入数据时隐式创建mapping，就是动态mapping。

## the deducing types mechanism

实际上就是根据编程语言的语义来，小数映射为float，整数映射为long，时间格式映射为date。

## limitations of dynamic mapping

dymamic mapping并不是完美的，有时可能推断错误，比如本意是存储一个类型为number的age字段，但是写数据的时候带上了引号，那么就被推断为text。

```json
PUT students_temp/_doc/1
{
  "name":"John",
  "age":"12"
}
```

当用age排序的时候就会报错，因为text不支持排序。

当然text也可以支持排序，有两种方法：第一种是需要在定义mapping的时候加上

```json
PUT students_with_fielddata_setting
{
  "mappings": {
    "properties": {
      "age": {
        "type": "text",
        "fielddata": true
      }
    }
  }
}
```

但计算过于昂贵不推荐。

第二种就是排序时用`age.keyword`，

```json
GET students_temp/_search 
{
  "sort":[{
    "age.keyword":{
      "order":"asc"
    }
  }]
}
```

因为你看mapping，其中text字段其实是有keyword的，这种被称为多字段（multi-field）。

```json
{
  "students_temp" : {
    "mappings" : {
      "properties" : {
        "age" : {
          "type" : "text",
          "fields" : {
            "keyword" : {
              "type" : "keyword",
              "ignore_above" : 256
            }
          }
        },
        "name" : {
          "type" : "text",
          "fields" : {
            "keyword" : {
              "type" : "keyword",
              "ignore_above" : 256
            }
          }
        }
      }
    }
  }
}
```

# Explicit Mapping

显式创建mapping的方式：

1. Indexing APIs
2. Mapping APIs



## Mapping using the indexing API

```json
PUT /employees
{
  "mappings": {
    "properties": {
      "name":{
        "type": "text"
      },
      "age": {
        "type": "integer"
      },
      "email": {
        "type": "keyword"
      },
      "address":{
        "properties": {
          "street":{
            "type": "text"
          },
          "country":{
            "type": "text" 
          }
        }
      }
    }
  }
}
```

adress是`object type`

## Updating schema using the mapping API

```json
PUT employees/_mapping
{
  "properties":{
    "joining_date":{
      "type":"date",
      "format":"dd-mm-yyyy"
    },
    "phone_number":{
      "type":"keyword"
    }
  }
}
```



## Modifying the existing fields is not allowed

不能修改已定义的字段的类型。

除非使用reindex，本质上就是创建一个新的再迁移数据。

1. 创建新索引
2. 复制数据：

```json
POST _reindex
{
  "source":{"index", "orders"},
  "dest":{"index", "orders_new"}
}
```

1. 应用切换索引
2. 删除旧索引



如果不想要应用切换旧索引，可以用别名。第六章讨论别名。

# Data Types

data types can be broadly classified under the following categories：

1. simple types：strings, dates, numbers, text, boolean, double, binary
2. complex type：object,
3. specialized types：geo, ip

类型：[https://www.elastic.co/guide/en/elasticsearch/reference/currhttps://www.elastic.co/guide/en/elasticsearch/reference/current/mapping-types.html](https://www.elastic.co/guide/en/elasticsearch/reference/current/mapping-types.html)

# Core Data Types

## the text data type

text字段存储前都会先进行分析。

可通过`_analyze`看到token。

```json
GET /_analyze
{
  "analyzer" : "standard",
  "text" : "Quick Brown Foxes!"
}
```

实际上text还有细分的类型，如`search_as_you_text, match_only_text, completion, token_count`，以`token_count`举例：

```json
PUT tech_books
{
  "mappings": {
    "properties": {
      "title": { 
        "type": "token_count", 
        "analyzer": "standard"
      }
    }
  }
}

PUT tech_books/_doc/1
{
  "title":"Elasticsearch in Action"
}
 
PUT tech_books/_doc/2
{
  "title":"Elasticsearch for Java Developers"
}
 
PUT tech_books/_doc/3
{
  "title":"Elastic Stack in Action"
}
```

因为存储了token_count，所以可以用单词个数进行过滤查询。

```json
GET tech_books/_search
{
  "query": {
   "range": {
     "title": {
       "gt": 3,
       "lte": 5
     }
   }
  }
}
```



可以将token_count作为多字段，这样就可以用字段数量来精确查询了：

```json
PUT tech_books2
{
  "mappings": {
    "properties": {
      "title": {
        "type": "text",
        "fields": { 
          "word_count": { 
            "type": "token_count", 
            "analyzer": "standard"
          }
        }
      }
    }
  }
}

GET tech_books/_search
{
  "query": {
    "term": {
     "title.word_count": {
        "value": 4
      }
    }
  }
}
```

## the keywords data types

keyword type不会被untokenized和analyzed，其实就是没有全文搜索的功能，只搜该词的部分是不行的。

1. the keyword type：`"field_name":{ "type": "keyword" }`
2. the constant_keyword type：设置为constant_keyword后，插入数据时可以不带有该字段，该字段将默认为设置的值 。

```
PUT census  {  "mappings": {    "properties": {      "country":{        "type": "constant_keyword",        "value":"United Kingdom"      }    }  } } PUT census/_doc/1 {  "name":"John Doe" }
```

1. the wildcard data type：通配符查询，谨慎使用
