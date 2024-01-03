1. Basic indexing operations
2. Index templating
3. Status management and monitoring
4. Index life-cycle management (ILM)

# Indexing operations

Index come with three sets of configurations:

1. settings
2. mappings
3. aliases

`GET <Index_name>`可以查看Index当这三个特性。

# Creating indices

隐式创建索引：没有预定义mapping，当插入第一个文档时，es会自动创建index，并使用dynamic mapping推断字段类型。

显式创建索引：插入数据前，预定义mapping。



## Creating indices implicity(automatic creation)

如果要禁用自动创建：

```json
PUT _cluster/settings 
{
  "persistent": {    
    "action.auto_create_index":false 
  }
}
```

`persistent`代表永久性的配置，即使重启也生效。`transient`在重启后就不再生效。

注意更改配置后，会导致后续的插入新索引的数据报异常。

不推荐修改，因为kibana也会创建一些隐藏索引，当然也可以设置为false后再排出一些以admin开头的索引，但这就比较麻烦了，不讨论这个了。

## Creating indices explicity

```json
PUT cars_index_with_sample_settings 
{
  "mappings": {
    "properties": {
      "make":{
        "type": "text"
      }
    }
  },
  "settings": {     
    "number_of_replicas": 3,
    "codec": "best_compression"
  },
  "aliases": {  
    "alias_for_cars_index_with_sample_alias": {} 
  }
}
```

## Index with custom settings

index setting以两种形式存在，一种是static，一种是dynamic settings。

static settings是那些索引在运行时无法修改的参数，比如分片个数、压缩算法、数据类型。

dynamic settings则相反，比如副本的个数、refresh intervals等。

见：https://www.elastic.co/guide/en/elasticsearch/reference/current/index-modules.html#index-modules-settings

```json
# Creating an index with custom settings
PUT cars_with_custom_settings 
{
  "settings":{ 
    "number_of_shards":3, 
    "number_of_replicas":5,  
    "codec": "best_compression",
    "max_script_fields":128,
    "refresh_interval": "60s"
  }
}

# Updating dynamic property on an index 
PUT cars_with_custom_settings/_settings
{
  "settings": {
    "number_of_replicas": 2
  }
}
```

## Index with mappings

```json
#Creating an index with field mappings for a car document
PUT cars_with_mappings
{
  "mappings": {  
    "properties": {  
      "make":{
        "type": "text" 
      },
      "model":{
        "type": "text"
      },
      "registration_year":{ 
        "type": "date",
        "format": "dd-MM-yyyy"  
      }
    }
  }
}
```

## Index with aliases

别名的作用：

1. 多个索引的搜索和聚合
2. re-indexing期间的零停机迁移

别名可以用来indexing, querying等在index操作的操作。

创建别名：

```json
# Creating an alias using an aliases object 
PUT cars_for_aliases 
{
  "aliases": {
    "my_new_cars_alias": {}
  }
}
```

创建还有单独的API：

```json
PUT|POST <index_name>/_alias/<alias_name>

PUT cars_for_aliases/_alias/my_cars_alias
```

不会覆盖之前的别名，而是新创建一个，共有两个别名：

```json
GET cars_for_aliases

# output 
{
  "cars_for_aliases" : {
    "aliases" : {
      "my_cars_alias" : { },
      "my_new_cars_alias" : { }
    },
    "mappings" : { },
    "settings" : {
      "index" : {
        "routing" : {
          "allocation" : {
            "include" : {
              "_tier_preference" : "data_content"
            }
          }
        },
        "number_of_shards" : "1",
        "provided_name" : "cars_for_aliases",
        "creation_date" : "1704183726492",
        "number_of_replicas" : "1",
        "uuid" : "XQMDpiG6S3K317ejpsgs9A",
        "version" : {
          "created" : "7170699"
        }
      }
    }
  }
}
```


单个别名可以同时指向多个index。

```
PUT cars1,cars2,cars3/_alias/multi_cars_alias
```




### migrating data with zero downtime using alias

1. Create an alias called to refer to the current index vintage_cars.
2. Because the new properties are incompatible with the existing index, create a new index, say with the new settings.
3. Copy (i.e., reindex) the data from the old index (vintage_cars) to the new index (vintage_cars_new).
4. Recreate your existing alias ( will now be pointed to vintage_cars_new.
5. Now all the queries that were executed against the are carried out on the new index.
6. Get rid of the old index (vintage_cars) when the reindexing and releasing works and is proven.


其实这里有些细节，比如不断有写入到旧索引，或者还包含了删除，如何保证新旧索引的数据是全的。

单纯的手段很难实现真正的零停机迁移，只能近似。



alias的一些API

```json
# Performing multiple aliasing operations   
POST _aliases 
{
  "actions": [
    {
      "remove": {
        "index": "vintage_cars",
        "alias": "vintage_cars_alias"
      }
    },
    {
      "add": { 
        "index": "vintage_cars_new",
        "alias": "vintage_cars_alias"
      }
    }
  ]  
}

# Creating an alias pointing to multiple indice
POST _aliases
{
  "actions": [
    {
      "add": {
        "indices": ["vintage_cars","power_cars","rare_cars","luxury_cars"],
        "alias": "high_end_cars_alais"
      }
    }
  ]
}
```

# Reading indices

## Reading public indices

```
GET cars1, cars2, cars3
GET ca*
```

## Reding hidden indices

hidden indices以`.`开头

```
GET _all or GET *
```

# Deleting indices

```
DELETE <index_name>
DELETE cars, movies, order
```

谨慎使用删除，这是直接删除索引，不仅仅是清空数据。

如果清空数据，使用`_delete_by_query`，第五章提到：

```json
POST movies/_delete_by_query
{
  "query": {
    "match_all": {}
  }
}
```

仅仅只删除别名：

```json
DELETE cars/_alias/cars_alias
```

# Closing and opening indices

## Closing indices

closing indices意味着暂时关闭索引的读写。

```
POST <index_name>/_close
```

关闭别名前，需要保证服务代码不再读写该索引，否则会报错。但如果使用别名，即使有遗漏的读写，也不会报错。



## Opening indices

```
POST <index_name>/_open
```

# Index templates

Elasticsearch 7.8版本后，索引模版可以归为两类：

1. composable index templates or simply index templates

1. 由0个或者多个component templates组成，

1. component templates


## Creating index templates

### Creating composable(index) tempaltes

```
index_template
# Creating an index template
POST _index_template/cars_template
{
  "index_patterns": ["*cars*"],
  "priority": 1, 
  "template": {
    "mappings": {
      "properties":{
        "created_at":{
          "type":"date"
        },
        "created_by":{
          "type":"text"
        }
      }
    }
  }
}
```

任何匹配到该模版的索引在创建时都会继承这个模版的设置，比如`new_cars, sports_cars.`

如果一个索引能够匹配上多个模版，最终会选择`priority`最高的那个模版。



### Creating component templates

component templates粒度更细，可以组合为index template提供配置，更具有复用性。

比如你有多于两套环境，环境只是setting配置不一样，但mappings是相同的，那就可以使用component进行组合。

```json
# Developing a component template 
POST _component_template/dev_settings_component_template
{
  "template":{
    "settings":{
      "number_of_shards":3,
      "number_of_replicas":3
    }
  }
}

# A component template with a mapping schema
POST _component_template/dev_mapping_component_template
{
  "template": {
    "mappings": {
      "properties": {
        "created_by": {
          "type": "text"
        },
        "version": {
          "type": "float"
        }
      }
    }
  }
}

# Creating an index template composed of a component template
POST _index_template/composed_cars_template
{
  "index_patterns": ["*cars*"], 
  "priority": 200, 
  "composed_of": ["dev_settings_component_template",
  "dev_mapping_component_template"]
}
```

# Monitoring and managing indices

## Index statistics

```
GET <index_name>/_stats
```

## multiple indices and statistics

```
GET <index_name1 >, <index_name2>/_stats
```

# Advanced operations

## Splitting an index

有时候索引的数据增长膨胀，超过硬件负载时就可能丢失数据，因此需要加硬件扩充主分片，就可以使用`_split`API，将旧的数据复制到新的索引上，这块内容不常用，先忽略细节

## Shrinking an index

当硬件资源过于充裕时，可能要缩减分片，那就要用`_shrink`，同理先忽略。

## rolling over an index alias

rollover 滚动索引，相当于是到达一定阈值后自动新增新的索引来管理新数据，旧索引将只读。

尤其是时序的数据，比如每天的数据都到一个单独的索引，比如`data-22-10, data-22-11`，

```json
#Prerequisite for the next query
PUT cars_2021-000001

# Creating an alias for an existing index 
POST _aliases 
{
  "actions": [ 
    {
      "add": { 
        "index": "cars_2021-000001", 
        "alias": "latest_cars_a",
        "is_write_index": true 
      }
    }
  ]
}

# The response from the above rollover call:
#{
#  "acknowledged" : true,
#  "shards_acknowledged" : true,
#  "old_index" : "latest_cars-000001", #A Old index name
#  "new_index" : "latest_cars-000002", #B New index name
#  "rolled_over" : true,
#  "dry_run" : false,
#  "conditions" : { }
#}

# Rolling over the index             
POST latest_cars_a/_rollover
```

`POST latest_cars_a/_rollover`意味着触发递增创建下一个index，新插入的文档将在最新的索引上，旧的索引将改为只读。



# Index life-cycle management(ILM)

滚动索引还可以进一步自动化：

1. 分片尺寸达到某个指定的阈值就自动创建
2. 达到某个时间就自动创建

Elasticsearch提供了这个特性，也就是Index life cycle management（ILM）。可以预先定义策略，比如：

1. 索引达到40G
2. 文档数达到10000
3. 每个天/月进行滚动

## Index life cycle


1. HOT：读写
2. WARM：只读
3. COLD：只读
4. FROZN：只读
