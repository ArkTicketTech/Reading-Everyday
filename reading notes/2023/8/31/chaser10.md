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
##### 写流程
WAL保证持久化 -> memtable -> 达到阈值写入磁盘 sstable

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
