####  5.2.4 std::atomic<T*>
原子指针类型，T 可以为内置类型或自定义类型。不能进行拷贝构造和拷贝赋值，可是通过合适的类型指针进行构造和赋值。
* std::atomic<T*> 有 load(), store(), exchange(), compare_exchange_weak()和 compare_exchage_strong()成员函数，均返回 T*。
* 提供了 fetch_add() 和 fetch_sub()，支持在存储地址上进行原子加法和减法，为 exchange-and-add 操作（交换-相加操作），为一个原子的 “读-改-写” 操作，因此可以使用任意的内存序。