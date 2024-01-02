the chapter covers：

1. Indexing, retrieving and reindexing documents
2. Manipulating and tweaking responses
3. updating and deleting documents with apis and query methods
4. working with scripted updates
5. indexing documents in bulk



# Indexing documents

The act of persisting the documents to these indexes is known as indexing.

documents在存储之前会经历text analysis，这个过程将会在第7章深入讨论。

## Document APIs

### Indexing a document with an identifier(PUT)

put语法：

```
PUT <index_name>/_doc/<identifier>
PUT movies/_doc/1 
{ 
  "title":"The Godfather", 
  "synopsis":"The aging patriarch of an organized crime dynasty transfers control of his clandestine empire to his reluctant son"
}
```

`<identifier>`就相当于primary key。


### Indexing a document without an identifier (POST)

put语法插入数据时要明确id，但post可以忽略（其实也可以带上），系统会自动创建uuid。

```json
POST myindex/_doc 
{
  "title" : "Elascticsearch In Action"
}
```


### Using _create to avoid overriding a document

put api可以对文档进行覆写，如果想避免错误的覆写带来的危害，可以使用`_create`，覆写时会报异常。

```json
PUT movies/_create/100
{
  "title":"Mission Impossible",
  "director":"James Cameron"
}
```



### Disabling index autocreation

```json
PUT _cluster/settings
{
  "persistent": {
    "action.auto_create_index": "false" 
  }
}
```

## Mechanics of indexing


变更的document首先是到memory buffer，Lucene's scheduler每1秒对memory buffer中的documents进行收集，然后创建一个新的段（segment，包含了文档数据和倒排索引）写进file system cache，最后再提交到磁盘。这样做的原因是因为I/O操作很昂贵。

lucene采用three-segments-merge pattern，每三个segment创建一个新的segment。

segment一旦创建，就不能修改，如果是删除文档，也只能进行标记，而修改会有版本号，其实也是新增的。




## customizing the refresh

可以随时动态配置refresh，

```json
PUT movies/_settings
{
  "index":{
    "refresh_interval":"60s"
  }
}
```

如果要关闭，就设置为-1。

### Client side refresh controls

还可以在客户端的CRUD请求中配置refresh：

1. PUT movies/_doc/1?refresh=false 使用默认配置1s
2. PUT movies/_doc/1?refresh=true 立刻刷新
3. PUT movies/_doc/1?refresh=wait_for 阻塞请求等待refresh成功



# Retrieving documents

## using the single document API

根据ID查询：`GET <index_name>/_doc/<id>`

判断id是否存在：`HEAD movies/_doc/1`

## retrieving multiple documents

使用multi-get(mget) api获取多条documents

```json
GET movies/_mget
{
  "ids" : ["1", "12", "19", "34"]
}
```

从多个索引获取文档，但每个索引只能查询一个id，这种查询可能会有性能问题。

```json
# Fetching docs from multiple indices using _mget endpoint
GET _mget 
{
  "docs":[
   {
     "_index":"classic_movies", 
     "_id":11
   },
   {
     "_index":"international_movies", 
     "_id":22
   },
   {
     "_index":"top100_movies",  
     "_id":33
   }
  ]
}
```



除了使用mget，还可以直接使用query api查询多条数据

```json
# Use ids query to fetch multiple documents 
GET classic_movies/_search
{
  "query": {
    "ids": {
      "values": [1,2,3,4]
    }
  }
}
```

如果要从多个索引查询同一个条件的文档数据：

```json
# Fetching multiple docs from multiple indices
GET classic_movies,international_movies/_search 
{ 
  # body
} 
```

# Manipulating responses

返回由metadata和source构成。

## Removing metadata from the response

如果只返回source，

```
GET <index_name>/_source/<id>
```



## Suppressing the source document

只需要metadata时

```
GET <index_name>/_doc/<id>?_source=false
```

## Including and excluding fields

```
GET movies/_doc/3?_source_includes=title,rating,genre
GET movies/_source/3?_source_excludes=actors,synopsis
GET movies/_source/13?_source_includes=rating*&_source_excludes=rating_amazon
```

`rating*`是通配符匹配，匹配所有以`rating`开头的字段，但是`source_excludes`排除掉`rating_amazon`。

# Updating documents

## Document update mechanics

update会经历

1. fetch the given documents
2. modify/add the fields
3. re-index the update document

update本质上就是添加一条新的文档来替代旧的文档，查询数据时可以看到返回数据是有版本号的，版本号就意味着更新的次数。当新的文档更新完成后，旧的版本会被es后台进行删除。这个机制跟多版本非常类似了。

## the update API

语法：`POST <index_name>/_update/<id>`

当我们想更新一条文档时，一般有这三种情况：

1. 增加新的字段；
2. 修改现有的字段值
3. 替换整个文档

```json
POST movies/_update/1
{
  "doc": {
    "actors":["Marlon Brando","Al Pacino","James Caan"],
    "director":"Francis Ford Coppola"  
  }
}
```

注意是直接覆盖旧字段的值，所以如果是数组，是需要包含旧值的。

## Scripted updates

除了上面的更新以外，还可以使用scripts来进行更新操作。scriptd update可以在更新前添加一些前提条件，比如达到一定的阈值才进行更新。

script使用`context variable - ctx`来进行数据获取，语法为`ctx._source.<field>`。

比如上面的actors数组，我们可以不需要旧值，只添加某个新值:

```json
POST movies/_update/1
{
  "script": {
    "source": "ctx._source.actors.add('Diane Keaton')"
  }
}
```

又或者我们删除某个值：

```json
POST movies/_update/1
{
  "script": {
    "source": "ctx._source.actors.remove(ctx._source.actors.indexOf('Diane Keaton'))"
  }
}
```

又或者我们要添加新字段

```json
### Adding multiple fields
POST movies/_update/1
{
  "script": {
    "source": """
    ctx._source.runtime_in_minutes = 175;
    ctx._source.metacritic_rating= 100;
    ctx._source.tomatometer = 97;
    ctx._source.boxoffice_gross_in_millions = 134.8;
    """
  }
}
```

又或者有条件的更新：

```json
### Conditional updates
POST movies/_update/1
{
  "script": {
    "source": """
    if(ctx._source.boxoffice_gross_in_millions > 125) 
      {ctx._source.blockbuster = true} 
     else 
      {ctx._source.blockbuster = false}
    """
  }
}
```



详细讲解下：


source是放置逻辑的地方，params是放置可配置动态参数的位置。

```json
### Dynamically passing a parameter to the script
POST movies/_update/1
{
  "script": {
    "source": """
 if(ctx._source.boxoffice_gross_in_millions > params.gross_earnings_threshold) 
   {ctx._source.blockbuster = true} 
 else 
   {ctx._source.blockbuster = false}
 """,
    "params": {
      "gross_earnings_threshold":150
    }
  }
}
```

Script默认的语言是Painless，Painless是一种es的脚本，也就是在source中写的代码。

```json
script": {
    "lang": "painless|mustache|expression|java",
    "source": "...",
    "params": { ... }
  }
```

## Replacing documents

上面讲到，第三种更新就是替换整个文档，其实就是普通的带id的写入，自然就会替换。

```json
PUT movies/_doc/1
{
  "title":”Avatar”
}
```

注意update时api中会有`_update`和`doc`，有所区别。

## Upsert

upsert就是update和insert。

如果数据存在就更新，不存在就新增。

```json
### Upsert
POST /movies/_update/5
{
  "script": {
    "source": "ctx._source.gross_earnings = '357.1m'"
  },
  "upsert": {
    "title":"Top Gun",
    "gross_earnings":"357.5m"
  }
}
```

如果id为5的文档不存在，就新增upsert中的数据，如果存在，就执行script中的更新。

如果不用script的更新语句，那需要`doc_as_upsert`关键字。

```json
POST movies/_update/11
{
  "doc": {
    "runtime_in_minutes":110  
  },
  "doc_as_upsert":true
}
```

## Updating by a query

匹配某个复杂条件查询的更新，要用到`_update_by_query`。

```json
POST movies/_update_by_query
{
  "query": {
    "match": {
      "actors": "Al Pacino"
    }
  },
  
  "script": {
    "source": """
    ctx._source.actors.add('Oscar Winner Al Pacino');
    ctx._source.actors.remove(ctx._source.actors.indexOf('Al Pacino'))
    """,
    "lang": "painless"
  }
}
```

# Deleting documents

删除有两种方式，通过id或者通过查询。

## Deleting with an ID

```
DELETE <index_name>/_doc/<id>
```

## Deleting by query



```json
POST movies/_delete_by_query
{
  "query":{
    "match":{
      "director":"James Cameron"
    }
  }
}
```

## Delete by range query

```json
# Delete the movies by a range query
POST movies/_delete_by_query
{
  "query": {
    "range": {
      "gross_earnings_in_millions": {
        "gt": 350,
        "lt": 400
      }
    }
  }
}
```

## Deleting all documents

```json
POST movies/_delete_by_query
{
  "query": {
    "match_all": {}
  }
}
```

# Working with documents in bulk

_bulk API接受POST请求，支持`index/create/delete/update`,



## format of the _bulk API


```json
POST _bulk
{
  "index|create/delete/update": {"_index":"<index_name>",	"_id":<identifier> }
  {"field1": "value", "field2": "value"}                               
}
```



## Bulk indexing documents

```json
POST _bulk
{"index":{"_index":"movies","_id":"100"}} 
{"title": "Mission Impossible","release_date": "1996-07-05"}
```

如果有明确的索引，我们可以将索引放到POST后面。

```json
POST movies/_bulk
{"index":{"_id":"100"}} 
{"title": "Mission Impossible","release_date": "1996-07-05"}
```

如果没有指定明确的id，用自动生成的uuid，那就不带上id：

```json
POST movies/_bulk
{"index":{}} 
{"title": "Mission Impossible","release_date": "1996-07-05"}
{"index":{}} 
{"title": "Mission Impossible II","release_date": "2000-05-24"}
{"index":{}} 
{"title": "Mission Impossible III","release_date": "2006-05-03"}
{"index":{}} 
{"title": "Mission Impossible - Ghost Protocol","release_date": "2011-12-26"}
```



## Independent entities and multiple actions

bulk可以实现多个索引的操作

```json
## Multiple entities

POST _bulk
{"index":{"_index":"books"}} 
{"title": "Elasticsearch in Action"}
{"create":{"_index":"flights", "_id":"101"}} 
{"title": "London to Bucharest"}
{"index":{"_index":"pets"}} 
{"name": "Milly","age_months": 18}
{"delete":{"_index":"movies", "_id":"101"}} 
{ "update" : {"_index":"movies", "_id":"1"} }
{ "doc" : {"title" : "The Godfather (Original)"} }
```

# Reindexing documents

如果我们要迁移某个索引的数据到另一个索引，可以使用`_reindex`。

```json
POST _reindex
{
  "source": {"index": "<source_index>"},
  "dest": {"index": "<destination_index>"}
}
```





```json
POST _reindex
{
  "source": {"index": "movies"},
  "dest": {"index": "movies_new"}
}
```

可以在生产上使用别名做到零停机迁移（zero downtime migration）。
