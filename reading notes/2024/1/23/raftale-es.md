GET _cat/nodes?v=true

node.role
1. d: data node
2. i: ingest node
3. r: remote cluster client node
4. m: master-eligible node
5. c: cold node 
6. f: frozon node
7. l: machine learning node
8. s: content code 
9. w: warm node
10. ...


不管是写入还是查询，都先经过协调节点，而不是master节点。
一般是让数据节点兼任协调节点。
协调节点拿到数据节点返回的数据，会做最后的统计或者聚合或者排序工作，然后返回给客户端。
如果查询条件比较复杂，那么协调节点做的事情就比较多。
不建议配置master节点为客户端节点和数据节点,master节点负责集群管理，要保证稳定。

refresh是将内存中的数据转变为segment写到文件系统缓存中，segment写成功后就可以支撑查询。
由于写操作是写的内存，所以还是存在丢数据的风险，因此es中也有transaction log，类似于wal日志，来保证数据的持久性。
