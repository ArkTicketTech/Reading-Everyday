#### 5.2.3 std::atomic<bool>

* std::atomic<bool> 比 std::atomic_flag 有更全的布尔标记特性，可以使用非原子类型的 bool 类型构造和赋值。
    * 非原子 bool 类型的复制操作：返回一个 bool 值来代替指定对象。不返回引用的原因：会导致任何依赖这个赋值结果的代码都需要显示加载。
* std::atomic<bool> 可以使用 store() 代替 std::atomic_flag 中的 clear()，test_and_set() 可以代替更加通用的 exchange()。
* std::atomic 支持对值的查找，会将对象隐式转换为普通 bool 值，或者是显示调用 load() 来完成。
* compare_exchange_weak() 和compare_exchange_strong()：当前值与预期值一致时，存储新值的操作。

 