##### TRANSITIVE SYNCHRONIZATION WITH ACQUIRE-RELEASE ORDERING
可使用 read-modify-write operation 操作（memory_order_acq_rel）同时进行获取和释放语义，每一个动作都包含了获取和释放操作，可以和之前的存储操作进行同步，并可以对随后的加载操作进行同步。

合并下列例子中的 2 和 3.

```c++
std::atomic<int> data[5];

std::atomic<bool> sync1(false), sync2(false);

void thread_1() {
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    sync1.store(true, std::memory_order_release); // 1.设置sync1
}

void thread_2() {
    while (!sync1.load(std::memory_order_acquire)); // 2.直到sync1设置后，循环结 束
    sync2.store(true, std::memory_order_release); // 3.设置sync2 
}

void thread_3() {
    while (!sync2.load(std::memory_order_acquire)); // 4.直到sync1设置后，循环结束
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
}
```

改写后：
```c++
std::atomic<int> sync(0);

void thread_1() {
    // ...
    sync.store(1, std::memory_order_release);
}

void thread_2() {
    int expected = 1;
    while (!sync.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))
        expected = 1;
}

```

##### DATA DEPENDENCY WITH ACQUIRE-RELEASE ORDERING AND MEMORY_ORDER_CONSUME
memory_order_consume 完全依赖于数据。
数据依赖：
* 前序依赖（dependency-ordered-before）
* 携带依赖（carries-a-dependency-to）：例如 A 操作的结果传递作为 B 操作的操作数。
如果不想要为携带以来增加其它开销，可以使用 `std::kill_dependecy()` 显示打破依赖，其用编译器在寄存器缓存这些值，并优化操作重排序操作代码。`std::kill_dependecy()` 本质上将实参拷贝。