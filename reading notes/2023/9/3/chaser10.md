哈希表用哈希函数将key分配到不同的bucket，如果两个元素被分配到了同一个bucket则称之为哈希碰撞，两种应对方法 1.链表法 也就是作为链表插入到这个bucket后面  2. 开放地址法 通过一定规律在bucket数组后面挑选空位

#### 数据结构

```
```golang
// A header for a Go map.
type hmap struct {
    // 元素个数，调用 len(map) 时，直接返回此值
	count     int
	flags     uint8
	// buckets 的对数 log_2
	B         uint8
	// overflow 的 bucket 近似数
	noverflow uint16
	// 计算 key 的哈希的时候会传入哈希函数
	hash0     uint32
    // 指向 buckets 数组，大小为 2^B
    // 如果元素个数为0，就为 nil
	buckets    unsafe.Pointer
	// 等量扩容的时候，buckets 长度和 oldbuckets 相等
	// 双倍扩容的时候，buckets 长度会是 oldbuckets 的两倍
	oldbuckets unsafe.Pointer
	// 指示扩容进度，小于此地址的 buckets 迁移完成
	nevacuate  uintptr
	extra *mapextra // optional fields
}
```

使用哈希函数计算key的哈希值 低B位决定key落向bucket数组中哪一个bucket  高位决定key落向bucket中哪一个cell  如果发生碰撞则将其放入到overflow bucket当中
查找时当前key没有被搬迁还得去老的bucket中寻找

#### 扩容
1. 增量扩容
	当元素数量/ bucket的数量 大于6.5时 触发增量扩容 此时bucket的数量翻倍
2. 等量扩容
	当map反复增删导致key 散落在overflow bucket， 搬迁后使bucket中的key更紧密
渐进式搬迁  不会一次都搬了 会在增删过程中触发搬迁过程 每次最多搬迁两个bucket

#### 为什么key是无序的
首先key是通过hash插入到bucket中的 遍历map的话就是遍历bucket数组 那么遍历bucket数组也只是通过哈希值的顺序来遍历 而不是通过插入顺序