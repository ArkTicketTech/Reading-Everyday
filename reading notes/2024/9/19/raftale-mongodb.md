
## MongoDB
MongoDB是一个流行的开源文档型数据库，它使用类似 JSON 的文档模型存储数据。

MongoDB使用集合来组织文档，每个文档都是键值对组成的。


## MySQL与MongoDB
| MySQL    | MongoDO    |
|----------|------------|
| database | database   |
| table    | Collection |
| row      | Document   |
| column   | Field      |
| index    | index      |

1. MySQL新增字段需要提前改表结果，MongoDB则直接插入数据即可，允许不同的文档有不同的字段
2. MySQL存储是row，MongoDB是文档JSON存储。
## MongoDB与ElasticSearch
mongoDB和ElasticSearch，它们两者有什么不同？
- Es的存储是采用的倒排索引和分词的方式
- MongoDB文档存储是采用的binary JSON，在BSON中，文档的键值对以键值对的方式存储

## 查询
```json
{
    "_id": 1,
    "name": "Alice",
    "age": 30,
    "gender": "female", 
    "organization": {
      "company": {
          "subCompany": ""
        }
    }
}
```

## 索引
默认对`_id`进行索引。
1. 单字段索引
2. 复合索引
3. 文本索引
4. 地理空间索引

对于嵌套的json字段，创建索引：
```js
db.collection.createIndex({ "organization.company.subCompany": 1 })
```
 
