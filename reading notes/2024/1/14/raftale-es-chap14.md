

1. Horizontally scaling the cluster
2. Internode communication
3. Shards and replica sizing
4. Working with snapshots and restoration
5. Understanding the master role in a culster

# Scaling the cluster

Elasticsearch是一个分布式系统，集群环境下可以伸缩节点。

## Adding node to the cluster

```json
PUT chats
{
  "settings": {
    "number_of_shards": 1, 
    "number_of_replicas": 1
  }
}
```

## Cluster health

```
GET _cluster/health
```

集群状态：

1. RED：Not all shards are assigned and ready(the cluster is being prepared)
2. YELLOW：Shards are assigned and ready, but replicas aren't assigned ready
3. GREEN：shards and replicas are all assigned and ready

```json
GET _cluster/allocation/explain
{
  "index": "chats",
  "shard": 0,
  "primary": false
}
```

## Increasing read throughput

增加副本可以提高性能。

副本能够提高读的吞吐量，主分片负责写，副本进行读操作。

```json
PUT chats/_settings
{
  "number_of_replicas": 10
}
```

# Node communication

1. The HTTP interface for interactions between clients and nodes using RESTful APIs. (We looked at these when running queries.)

1. 默认端口号9200

1. The transport layer interface for node-to-node communications.

1. 默认端口号9300

# Shard sizing



## Setting up a single index



![img](https://cdn.nlark.com/yuque/0/2024/png/32473878/1705230491713-dc5400ac-62db-4996-aedf-5368989db62f.png)



![img](https://cdn.nlark.com/yuque/0/2024/png/32473878/1705230622636-cd4f42df-0a2c-45ac-8d70-b4df2af57275.png)

## Settting up multiple indexes

# Snapshots

snapshots用作备份和恢复

![img](https://cdn.nlark.com/yuque/0/2024/png/32473878/1705230942869-44fe7428-398a-42c1-9a6b-d905b92bf835.png)

## Getting started

1. Register a snapshot repository：HDFS or cloud storage such as Amazon S3 buckets
2. Snapshot the data
3. Restore from the store



Registering a snapshot repository

Creating snapshots

Restoring snapshots

Deleting snapshots

Automating snapshots





# Advanced configurations

有三个配置文件可以修改：

1. elsticsearch.yml：集群相关信息
2. log4j2.properties：日志级别
3. jvm.options：运行节点的堆内存

# Cluster masters



## Master nodes

## Master elections

## Cluster state

## A quorum

过半

## The split-brain problme

脑裂



本章内容讲的很浅
