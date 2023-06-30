

| 概念 | 描述 |
| --- | --- |
| Sleep 和 Wakeup | 这两种机制允许内核线程等待特定的事件；另一个线程可以调用wakeup来表示等待事件的线程应该恢复。这两个机制经常被称为序列协调或条件同步机制。 |
| 信号量 (Semaphore) | 是一个维持计数并提供两种操作的高级同步机制。"V"操作（对于生产者）增加计数。"P"操作（对于消费者）等待直到计数为非零，然后减少计数并返回。 |
| Sleep 和 Wakeup 的实现 | Sleep(chan)在任意值chan上休眠，称为等待通道。Sleep使调用进程进入休眠，释放CPU以进行其他工作。Wakeup(chan)唤醒所有在chan上休眠的进程（如果有的话），使他们的sleep调用返回。如果没有进程在chan上等待，wakeup什么也不做。 |
| 忙等待的避免 | 需要一种方式使消费者在V增加计数后放弃CPU并恢复，避免忙等待。 |
| Lost wake-up 问题 | 当P发现s->count == 0时，假设在P执行行212和213之间，V在另一个CPU上运行：它改变s->count为非零并调用wakeup，这样找不到正在睡眠的进程，因此不做任何事情。现在P继续在行213执行：它调用sleep并进入睡眠。这会导致问题：P在等待已经发生的V调用而睡着。 |
| 死锁的问题 | 如果P在睡眠时保持锁定，那么V将无限期地等待锁。 |
| 解决方案 | 调用者必须将条件锁传递给sleep，以便在将调用进程标记为睡眠并在等待通道上等待后释放锁。这将迫使并发的V等待，直到P完成使自己进入睡眠，以便wakeup能找到睡眠的消费者并唤醒它。 |


| 概念 | 描述 |
| --- | --- |
| Sleep (kernel/proc.c:529) and Wakeup (kernel/proc.c:560) in xv6 | Sleep函数标记当前进程为SLEEPING然后调用sched释放CPU；Wakeup函数查找在给定等待通道上睡眠的进程，并将其标记为RUNNABLE。 |
| p->lock and lk | Sleep获取p->lock，即将要进入休眠的进程现在持有p->lock和lk。在调用者中持有lk是必要的：它保证了没有其他进程可以开始调用wakeup(chan)。|
| Putting the process to sleep | sleep现在持有p->lock，所以可以通过记录睡眠通道，改变进程状态为SLEEPING，然后调用sched，将进程放入睡眠状态。|
| Wakeup operation | 在某个时候，一个进程会获取条件锁，设置sleeper正在等待的条件，并调用wakeup(chan)。这很重要，wakeup在持有条件锁的情况下被调用。 |
| Locking rules for sleep and wakeup | 确保睡眠进程不会错过wakeup的原因是，睡眠进程从检查条件之前的点到标记为SLEEPING之后的点，要么持有条件锁，要么持有自己的p->lock，或者两者都有。在wakeup的循环中，调用wakeup的进程会持有这两个锁。|
| Multiple processes sleeping on the same channel | 这种情况下，一个wakeup调用将唤醒所有睡眠的进程。其中一个将首先运行并获取sleep被调用时的锁，并（在管道的情况下）读取等待在管道中的任何数据。其他进程会发现，尽管被唤醒，但没有数据可读。|
| Spurious wakeups | 如果两次使用sleep/wakeup不小心选择了同一个通道，那么它们会看到伪唤醒，但如上述所述的循环将容忍这个问题。sleep/wakeup的魅力在于它既轻量级（无需创建特殊的数据结构充当睡眠通道）又提供了一层间接性（调用者不需要知道他们正在与哪个具体的进程交互）。|
