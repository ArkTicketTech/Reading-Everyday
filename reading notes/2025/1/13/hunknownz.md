在分布式系统的算法和设计中，safety和liveness是2个非常重要的属性，这个概念最早由L. Lamport1提出。这2个属性是非常基础的属性，系统中的其他属性都可以被分解为safety和liveness。

通俗来讲，这2个属性的含义就是2：

safety something “bad” will never happen
liveness something “good” will must happen (but we don’t know when)
我们可以举一些例子来说明：

同一时刻，只有一个进程能够进入互斥临界区
1. 是一个关于safety的表述，它表述了，什么样的时间不应该被发生，那就是”同一时刻，不少于一个进程进入了互斥临界区“。

进程P2不会永远停留在互斥临界区，以至于P1最终能够进入互斥临界区。
2. 是一个关于liveness的表述，它表述了什么样的事件最终应该被发送，那就是“P1最终会进入互斥临界区”。

gossip协议具有最终一致性
3是关于liveness的表述，

更加形式化的解释我们可以参考Safety & Liveness Properties3。

虽然safety和liveness是正交地两个属性，但是在设计一个分布式系统时，我们需要同时考虑这两个属性，只具备其中之一的系统是没有意义的4。

避免死锁是保证liveness的一个充分条件，虽然这个约束比较弱。但是是比较好验证的，如果算法、逻辑中存在死锁，那么一定不能保证系统的liveness。避免饥饿也是保证liveness的一个充分条件，这个约束比避免死锁更强一些，因为避免饥饿的算法一定是避免死锁的。更强的约束是，保证算法、逻辑能在有限步骤内完成，这样系统一定是liveness的，但是这个约束条件很难被验证和证明。

