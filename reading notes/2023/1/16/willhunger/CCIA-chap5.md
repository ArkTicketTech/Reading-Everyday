#### 5.2.2 std::atmoic_flag
* std::atomic_flag 类型的对象必须被 ATOMIC_FLAG_INIT 初始化，初始化标识位是 “清除” 状态。
* 初始化后，只能进行销毁，清除或设置(查询之前的值) 操作，分别为：clear() 和 test_and_set() 。
* 为保证操作原子性，不能拷贝构造和赋值 std::atomic_flag 对象。
* 可用于实现自旋锁。