压缩列表（ziplist）是列表对象和哈希对象的底层实现之一。

压缩列表是为了节约内存而开发的，是由一系列特殊编码的**连续内存块**组成的顺序型数据结构。



如图：



# 压缩列表节点

每个压缩列表节点可以保存一个字节数组或者一个整数值。

1. previous_entry_length: 记录了前一个节点的长度，单位为字节。借助于该参数，可以使得遍历操作从表尾往表头遍历。
2. encoding：记录了节点的content属性所保存数据的类型以及长度
3. content：负责保存节点的值，可以是一个字节数组或者整数

# 连锁更新

连锁更新是指在队头新增或者删除，可能导致大多数节点的previous_entry_length占用空间的变化。

需要注意的是：连锁更新的触发几率低，几乎不会造成性能问题，可以放心使用。

这个不是太重要，了解即可。



redis5.0 引入了listpack，是对ziplist的改进版本，在存储与结构上都比ziplist要更为节省和精简

```c
struct listpack<T> {
    int32 total_bytes; // 占用的总字节数
    int16 size;        // 元素个数
    T[] entries;       // 元素列表
    int8 end;          // 同ziplist的zlend一样，恒为0xFF
}
# entrie 机构如下
struct lpentry {
    int<var> encoding;
    optional byte[] content;
    int<var> length;
}
```
