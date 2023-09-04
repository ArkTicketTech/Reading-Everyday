#### 从Bitcask说起
Bitcask大体由两部分组成
1. 内存中索引 用于描述一条k,v数据在文件中的位置
2. 日志文件 追加(顺序)写以获取高性能

最主要的优势：
1. 写数据仅需顺序写WAL+更新内存索引
2. 读数据仅需从内存拿去位置+一次磁盘io  

存在着两个问题：
1. 内存需要维护完整的key+position信息 数据量很大的情况会非常占内存
2. key无序 进行范围查找得遍历整个哈希表

#### 整体架构
##### 写操作
WAL保证持久化，原子性 -> memtable -> 达到阈值写入磁盘 sstable

数据存储结构![[Pasted image 20230831223535.png]]
其中type为操作类型可能是删除也可能是正常写

key编码
![[Pasted image 20230831223905.png]]
seq做版本控制  一次batch操作seq累加一次 

**合并写**  在写入进程较多的情况下合并小写入
- 第一个写入操作获取写入锁
- 后续写入能合并则合并
- 超过合并上限或没有pending的写请求，将所有内容统一进行写入
- 通知写入结果，释放锁

```
// db_write
// 并发的Put协程
func (db *DB) putRec(kt keyType, key, value []byte, wo *opt.WriteOptions) error {
	if err := db.ok(); err != nil {
		return err
	}

	merge := !wo.GetNoWriteMerge() && !db.s.o.GetNoWriteMerge()
	sync := wo.GetSync() && !db.s.o.GetNoSync()

	// Acquire write lock.
	if merge {
		select {
		case db.writeMergeC <- writeMerge{sync: sync, keyType: kt, key: key, value: value}:
			// 此时可以合并小写入
			// 阻塞等待合并完成
			if <-db.writeMergedC {
				// Write is merged.
				return <-db.writeAckC
			}
			// Write is not merged, the write lock is handed to us. Continue.
		case db.writeLockC <- struct{}{}:
			// 前面没有正在合并的写入
			// Write lock acquired.
		case err := <-db.compPerErrC:
			// Compaction error.
			return err
		case <-db.closeC:
			// Closed
			return ErrClosed
		}
	}
	...
	// 获取锁之后去等待其他写入操作合并
	return db.writeLocked(batch, batch, merge, sync)
}

// 写协程 只有一个
// ourBatch is batch that we can modify.
func (db *DB) writeLocked(batch, ourBatch *Batch, merge, sync bool) error {
	// Try to flush memdb. This method would also trying to throttle writes
	// if it is too fast and compaction cannot catch-up.
	...
	merge:
		for mergeLimit > 0 {
			select {
			case incoming := <-db.writeMergeC:
				// 收到合并过来的写请求
				if incoming.batch != nil {
					// Merge batch.
					// 超过上限
					if incoming.batch.internalLen > mergeLimit {
						overflow = true
						break merge
					}
					batches = append(batches, incoming.batch)
					mergeLimit -= incoming.batch.internalLen
				} else {
					// Merge put.
					internalLen := len(incoming.key) + len(incoming.value) + 8
					if internalLen > mergeLimit {
						overflow = true
						break merge
					}
					if ourBatch == nil {
						ourBatch = db.batchPool.Get().(*Batch)
						ourBatch.Reset()
						batches = append(batches, ourBatch)
					}
					// We can use same batch since concurrent write doesn't
					// guarantee write order.
					ourBatch.appendRec(incoming.keyType, incoming.key, incoming.value)
					mergeLimit -= internalLen
				}
				sync = sync || incoming.sync
				merged++
				db.writeMergedC <- true

			default:
				// 没有要合并的
				break merge
			}
		}
	}
	// 增加序列号 用于版本控制
	db.addSeq(uint64(batchesLen(batches)))
	...
	// 释放锁
	db.unlockWrite(overflow, merged, nil)
	return nil
}

func (db *DB) unlockWrite(overflow bool, merged int, err error) {
	// 挨个通知写入结果
	for i := 0; i < merged; i++ {
		db.writeAckC <- err
	}
	if overflow {
		// Pass lock to the next write (that failed to merge).
		db.writeMergedC <- false
	} else {
		// Release lock.
		<-db.writeLockC
	}
}
```

##### 读操作
在读时会创建一个快照

```
// db line 861
func (db *DB) Get(key []byte, ro *opt.ReadOptions) (value []byte, err error) {
	err = db.ok()
	if err != nil {
		return
	}

	se := db.acquireSnapshot()
	defer db.releaseSnapshot(se)
	return db.get(nil, nil, key, se.seq, ro)
}

// db_snapshot
// Acquires a snapshot, based on latest sequence.
func (db *DB) acquireSnapshot() *snapshotElement {
	db.snapsMu.Lock()
	defer db.snapsMu.Unlock()

	seq := db.getSeq()
	// 获取所有活跃的快照
	if e := db.snapsList.Back(); e != nil {
		se := e.Value.(*snapshotElement)
		if se.seq == seq {
			se.ref++
			return se
		} else if seq < se.seq {
			panic("leveldb: sequence number is not increasing")
		}
	}
	se := &snapshotElement{seq: seq, ref: 1}
	se.e = db.snapsList.PushBack(se)
	return se
}
```

快照使用seq一个数字进行维护，一个序列号就代表者db的一个状态，获取到seq之后将ukey与seq、type编码。然后就能滤掉seq大于快照号的数据项。也就是快照隔离，快照隔离解决了脏读、不可重复读、幻读的问题 但是还存在着**写偏序**的问题

写偏序是指两个事务并发基于自己读到的数据集去覆盖另一部分数据集
[基于快照隔离的多版本并发控制方案 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/515281379)
![[Pasted image 20230901141523.png]]
事务A查询出下面两个球是白色的要给下面两个球染成黑色  事务B查询出上面两个球是黑色的要给后面两个球染成白色  最后结果是上面是白色 下面是黑色


```
key：黑球 value：2
key：白球 value：2
事务A  黑变白：
快照读读取黑球数量 2
黑球数量-2
白球数量+2

事务B 将白球变成黑球：
快照读读取白球数量2
白球数量-2
黑球数量+2

最后黑球数量2 白球数量2  但是串行化要求黑球4或者白球4
```


上面这种不可串行化的的本质是并发事务更新了各自进行数据项更新所有依赖的数据项

mysql中提供了for update语句将一开始的读操作标记为写操作就能保证串行化

```
key：黑球 value：2
key：白球 value：2
事务A  黑变白：
获取锁
读取黑球数量 2
黑球数量-2
白球数量+2

事务B 将白球变成黑球：
阻塞等待获取锁
读取白球数量4
白球数量-4
黑球数量+4

```

言归正传，读取分为三步
1. 在memory db中查找指定的key，若搜索到符合条件的数据项就返回
2. 从冻结的memorydb查找
3. 从新到旧的sstable中查找key
```
//DB Line 781
func (db *DB) get(auxm *memdb.DB, auxt tFiles, key []byte, seq uint64, ro *opt.ReadOptions) (value []byte, err error) {
	ikey := makeInternalKey(nil, key, seq, keyTypeSeek)

	if auxm != nil {
		if ok, mv, me := memGet(auxm, ikey, db.s.icmp); ok {
			return append([]byte(nil), mv...), me
		}
	}
	// 获取memorydb 和冻结的memorydb
	em, fm := db.getMems()
	for _, m := range [...]*memDB{em, fm} {
		if m == nil {
			continue
		}
		defer m.decref()

		if ok, mv, me := memGet(m.DB, ikey, db.s.icmp); ok {
			return append([]byte(nil), mv...), me
		}
	}

	v := db.s.version()
	// 去sstable中找
	value, cSched, err := v.get(auxt, ikey, ro, false)
	v.release()
	if cSched {
		// Trigger table compaction.
		db.compTrigger(db.tcompCmdC)
	}
	return
}
```

##### 日志
WAL保证 原子性
![[Pasted image 20230901154632.png]]
leveldb中有两个memorydb，每个db都对应一个log 当前活跃的db达到阈值就转化成只读的db 只读的db由后台进程持久化到sstable当中 当持久化完成删除log
###### 日志结构
日志文件时按照block存储，一个block包含一个或多个chunk   一个chunk对应一条记录或者多个chunk对应一个记录
![[Pasted image 20230901160127.png]]
分block为什么能提高效率呢？
假设不用block长度 那么首先我需要先把length读取到内存中然后再根据length再去读取固定长度的数据就起码要两次磁盘io  分块存储的话就可以一次读取固定大小的数据然后放到内存中继续处理

###### 日志内容
![[Pasted image 20230901163638.png]]
一条日志记录的内容包含：

- Header
- Data

其中header中有（1）当前db的sequence number（2）本次日志记录中所包含的写操作的个数。

![[Pasted image 20230901161538.png]]
![[Pasted image 20230901161934.png]]


```
func (db *DB) writeJournal(batches []*Batch, seq uint64, sync bool) error {
	// 获取一个singlewriter 用于写一条记录
	wr, err := db.journal.Next()
	if err != nil {
		return err
	}
	if err := writeBatchesWithHeader(wr, batches, seq); err != nil {
		return err
	}
	if err := db.journal.Flush(); err != nil {
		return err
	}
	if sync {
		return db.journalWriter.Sync()
	}
	return nil
}

func (x singleWriter) Write(p []byte) (int, error) {
	w := x.w
	if w.seq != x.seq {
		return 0, errors.New("leveldb/journal: stale writer")
	}
	if w.err != nil {
		return 0, w.err
	}
	n0 := len(p)
	for len(p) > 0 {
		// 此block已经满了.
		if w.j == blockSize {
			w.fillHeader(false)
			w.writeBlock()
			if w.err != nil {
				return 0, w.err
			}
			w.first = false
		}
		// Copy bytes into the buffer.
		n := copy(w.buf[w.j:], p)
		w.j += n
		p = p[n:]
	}
	return n0, nil
}
```

##### 内存数据库 memtable
内存数据库本质是个有序的k、v集合
internalkey由三部分组成
1. uKey 2. seq 3. type
按照这个顺序编码 那么当key不同时按字典序排序 相同时按seq排序 这样子就能以很快速度滤掉大于此时batch seq的数据 实现si
###### 数据组织

以goleveldb为示例，内存数据库的定义如下：


```
type DB struct {
    cmp comparer.BasicComparer
    rnd *rand.Rand

    mu     sync.RWMutex
    kvData []byte
    // Node data:
    // [0]         : KV offset
    // [1]         : Key length
    // [2]         : Value length
    // [3]         : Height
    // [3..height] : Next nodes
    nodeData  []int
    prevNode  [tMaxHeight]int
    maxHeight int
    n         int
    kvSize    int
}
```


其中kvData用来存储每一条数据项的key-value数据，nodeData用来存储每个跳表节点的**链接信息**。

nodeData中，每个跳表节点占用一段连续的存储空间，每一个字节分别用来存储特定的跳表节点信息。

- 第一个字节用来存储本节点key-value数据在kvData中对应的偏移量；
- 第二个字节用来存储本节点key值长度；
- 第三个字节用来存储本节点value值长度；
- 第四个字节用来存储本节点的层高；
- 第五个字节开始，用来存储每一层对应的下一个节点的索引值；

#### sstable
一个sstable文件按照4kb大小进行划分  物理结构除了存储data以外还会存储压缩类型和crc防止数据损坏
![[Pasted image 20230904125426.png]]
此外还存在元信息  一个sstable的元信息包含了 最大key以及最小key 以及sstable的大小
##### data block
datablock的data部分 由若干kv对组成  不会为每一对keyvalue存储完整的key值，而是存储与上一个key非共享的部分  ukey可能都是相同的 但是seq和type就是非共享的部分  每间隔若干kv对 就会存储一个restart point节点 他完整的存储了key 这个是用来加速查找的 因为记录了完整的key  我们可以很快查到我们要查的key大致在哪个范围
![[Pasted image 20230904125655.png]]
一个entry分为5部分内容：

1. 与前一条记录key共享部分的长度；
2. 与前一条记录key不共享部分的长度；
3. value长度；
4. 与前一条记录key非共享的内容；
5. value内容；
![[Pasted image 20230904130456.png]]

##### filter block
在直接查询data block前，leveldb首先根据filter block中的过滤数据判断指定的datablock中是否由需要查询的数据，若判断不存在，就不需对这个datablock进行查找

filter block存储的是data block数据的一些过滤信息
![[Pasted image 20230904131148.png]]
filter block中存储了两部分数据 1. 过滤数据 2.索引数据
在读取filter block中的内容是，可以先读出filter offset's offset的值 然后依次读取filter i oofset 然后根据offset读取 filter data

**一个sstable只有一个filter block**

找到filter block的位置信息是通过meta index block这个block只存储了filter在sstable的位置信息
##### index block
存储所有block的索引信息  用于快速寻找到data block
一条索引包括
1. data block i中最大的key值
2. 该data block起始地址
3. data block的大小![[Pasted image 20230904131911.png]]

##### footer
用于找到index block 和 meta index block
#### 写操作
sstable的写操作发生在以下两种场景
1. memory db将内容持久化到磁盘文件中，会创建一个sstable进行写入
2. 进行compaction时会将若干个sstable的内容进行重新组织，输出到新的sstable文件中

对sstable进行写操作的数据结构为tWriter

```
// tWriter wraps the table writer. It keep track of file descriptor
// and added key range.
type tWriter struct {
    t *tOps

    fd storage.FileDesc // 文件描述符
    w  storage.Writer   // 文件系统writer
    tw *table.Writer

    first, last []byte
}

```

**一次sstable的写入为一次不断利用迭代器读取需要写入的数据，并不断调用tableWriter的append函数 直到读完所有数据 为sstable文件附上元数据**
该迭代器可以是一个内存数据库的迭代器，写入情景对应着上述的第一种情况；

该迭代器也可以是一个sstable文件的迭代器，写入情景对应着上述的第二种情况；


```
// Writer is a table writer.
type Writer struct {
    writer io.Writer
    // Options
    blockSize   int // 默认是4KiB

    dataBlock   blockWriter // data块Writer
    indexBlock  blockWriter // indexBlock块Writer
    filterBlock filterWriter // filter块Writer
    pendingBH   blockHandle
    offset      uint64
    nEntries    int // key-value键值对个数
}
```

#### 读数据
1. 从cache中获取 key对应的sstable元信息和文件句柄  元信息包括了sstable的最大key和最小key以及sstable的大小
2. cache中没有的话就去sstable的index block中寻找最有可能所在的两个block  index block中记录了每个block的最大key以及其在sstable的偏移值
3. 从filter block中获取找到的data block的filter信息
4. 如果可能存在的话去两个block中迭代查找