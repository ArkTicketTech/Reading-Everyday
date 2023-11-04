## 读写锁

### ReadWriteLock

通过一个ReadWriteLock产生两个锁：一个读锁，一个写锁。读操作使用读锁，写操作使用写锁。需要注意的是，只有“读-读”操作是可以并行的，“读-写”和“写-写”都不可以。

A `ReadWriteLock` maintains a pair of associated `locks`, one for read-only operations and one for writing. The`read lock` may be held simultaneously by multiple reader threads, so long as there are no writers. The `write lock` is exclusive.

All `ReadWriteLock` implementations must guarantee that the memory synchronization effects of `writeLock`operations (as specified in the `Lock` interface) also hold with respect to the associated `readLock`. That is, a thread successfully acquiring the read lock will see all updates made upon previous release of the write lock.

### ReentrantReadWriteLock

一个ReentrantReadWriteLock同时只能存在一个写锁但是可以存在多个读锁，但不能同时存在写锁和读锁。也即一个资源可以被`多个读操作`访问或`一个写操作`访问，但两者不能同时进行。

只有在读多写少情境之下，读写锁才具有较高的性能体现。

