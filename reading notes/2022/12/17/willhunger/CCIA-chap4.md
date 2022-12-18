## Chap4 同步操作
我们需要对线程进行同步，例如，在一个线程完成前，等待另一个线程执行完成。C++ 标准库提供了条件变量（condition variable）和 future 用于同步，future 可用于新工具 **锁存器（latches）** 和 **屏障（barriers）** 一起使用。

### Chap4.1 等待事件或条件
线程等待另一线程完成的方式：
* 忙等，持续性的检查互斥量：消耗系统资源；
* 忙等中加入休眠：休眠时间太短导致浪费资源，休眠时间太长导致等待过久；
* C++ 标准库提供的工具，条件变量来唤醒。

#### 4.1.1 等待条件达成

`std::condition_variable` 和 `std::condition_variable_any` :
* 前者仅能和 `std::mutex` 一同工作，后者可以和合适的互斥量一起工作
* 后者更加通用，但是性能和系统资源的使用方面的开销会更大

```cpp
std::mutex mut;
std::queue<data_chunk> data_queue; // 1
std::condition_variable data_cond;

void data_preparation_thread()
{
	while(more_data_to_prepare())
	{
		data_chunk const data=prepare_data();
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data); // 2
		data_cond.notify_one(); // 3
	}
}

void data_processing_thread()
{
	while(true)
	{
		std::unique_lock<std::mutex> lk(mut); // 4
		data_cond.wait(lk,[]{return !data_queue.empty();}); // 5
		data_chunk data=data_queue.front();
		data_queue.pop();
		lk.unlock(); // 6
		process(data);
		if(is_last_chunk(data))
			break;
	}
}
```

wait 会检查条件，如果条件不满足，wait 则解锁互斥量，并且讲线程置于阻塞或等待状态，重新等待。因此，wait 条件不满足时，需要对互斥量解锁，需要使用 `std::unique_lock`。

调用 wait 的过程中，在互斥量锁定时，可能会去检查条件变量若干次，当提供测试条件的函数返回 true 就立刻返回。当等待线程重新获取互斥量并检查条件变量时，并非响应另一个线程的通知，而有可能发生虚假唤醒（spurious wakeup）。