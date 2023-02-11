#### 5.2.7 原子操作的非成员函数
命名规律：
* 以 `atomic_` 作为前缀，这类成员函数一般回重载不同的原子类型，例如 `std::atomic_load()`；
* 以 `_explicit` 作为后缀，用于指定内存序，例如 `std::atomic_store(&atomic_val, new_value)` 与  `std::atomic_store_explicit(&atomic_val, new_value, std::memory_order_release)`；
* 成员函数隐式引用原子对象，非成员函数都持有一个指向原子对象的指针。例如，成员函数 `is_lock_free` 和 `std::atomic_is_lock_free`。
* 非成员函数为了与 C 兼容，由于 C 中没有引用。非成员函数一般传递指针，而成员函数传递引用。
* `std::shared_ptr<>` 不是原子类型，但亦能使用 load, store, exchange和compare/exchange 等非成员函数的原子操作。
* Concurrency TS 中，提供了 `std::experimental::atomic_shared_ptr<T>` ，并可以通过 is_lock_free 在特定硬件平台上检查实现是否无锁。当实现不是无锁结构时，推荐使用 std::experimental::atomic_shared_ptr 原子函数，因为该类型会让代码更加清晰，确保所有的访问都是原子的，并且能避免由于忘记使用原子函数导致的数据竞争。