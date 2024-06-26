索引管理：

1. setting
2. mapping
3. aliases：
   - 单个别名可以指向多个索引，可以为查询提供便利
   - 零停机迁移数据

索引模版：
index template 由component template组成，能够简化重复的定义。

索引周期管理（ILM）：
滚动索引可以自动创建索引：
1. 分片尺寸达到某个指定的阈值就自动创建
2. 文档数达到某个阈值
3. 达到某个时间就自动创建

Index life cycle：
1. HOT：读写
2. WARM：只读
3. COLD：只读
4. FROZEN：只读，索引停止更新
5. DELETE：永久删除


hot->frozen的性能可能是越来越差的，如果硬件上有做区分的话。 
这五个阶段是可以组合的，并不一定都需要。


# text analysis
指分词并存储的过程，能够有助于更便捷的搜索，如同义词搜索、缩写、各国语言等。这个过程通过analyzer实现。
analyzer module包含了两个功能：
1. tokenization: 拆分词
2. normalization: 对拆分后的词做一些关联

analyzer的三个组件：
1. character filters：过滤掉一些不必要的character
2. tokenizers：上面的tokenization
3. token filters：上面的normalization

analyzer有内置的，也可以自定义。

# search
每个es节点都是协调者的角色，意味着外部请求可以打到任意一个节点，然后由节点进行路由。

Elasticsearch在内部搜索时使用执行上下文，执行上下文有：
1. query context：相关性搜索匹配
2. filter context：只匹配，没有相关性搜索
   
filter 因为没有相关性搜索，所以性能比query好。

bool query：
1. must
2. must not
3. should 
4. filter

## term
term-level query不会经过analyzer，它不会对文本进行分析，适用于keyword的字段。
1. 判断字段是否存在：exists
2. 批量id查询：ids
3. 前缀查询：prefix
4. 分词匹配：term
5. 多个分词匹配（分词之间的关系是or）：terms
6. 通配符：wildcard
7. 范围查询：range
8. 正则查询：regexp
9. 拼写错误匹配（编辑距离定义）：fuzzy

# full text

1. match query
   1. 单个词匹配：
   2. 多个词匹配：可以设置逻辑关系为and 或者or，默认为or
   3. 控制match的匹配百分比：查询多个词中至少匹配某个百分比的文档，minimum_should_match，
   4. 精确包含某句话：match_phrase
2. query string：将查询规则写入一个string，只通过这个复杂string来查询。
3. interval：将多个规则按照顺序匹配





