##### ACQUIRE-RELEASE ORDERING
acquire-release ：
* atomic loads are acquire operations (memory_order_acquire)
* atomic stores are release operations (memory_order_release)
* atomic read-modify-write operations (such as fetch_add() or exchange()) are either acquire, release, or both (memory_order_ acq_rel)
* Synchronization is pairwise between the thread that does the release and the thread that does the acquire. **A release operation synchronizes-with an acquire operation that reads the value written.**

为了同步，acuqire 和 release 操作必须成对出现，release 存储的值必须让 acquire 获取操作看见。