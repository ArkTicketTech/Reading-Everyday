
标准原子类型：
* 不能进行拷贝和复制，无拷贝构造函数和拷贝赋值操作符；但可以进行隐式转换为对应的内置类型。
* 可以使用 `load()` 和 `store()`、`exchange()`、`compare_exchange_weak()` 和 `compare_exchange_strong()`；且都支持复合赋值符：`+=`, `-=`,`*=`,`/=` 等。
* 使用整形和指针的特化类型支持自增运算符和自减运算符；且有功能相同的成员函数所对应：`fetch_add()` , `fetch_or()` 等。

原子类型的操作都可以规定内存序参数，类别如下：
1. Store 操作：memory_order_relaxed、memory_order_release、memory_order_seq_cst。
2. Load 操作：memory_order_relaxed、memory_order_consume、memory_order_acquire、memory_order_seq_sct。
3. Read-Modify-Write （读-改-写）操作：memory_order_relaxed、memory_order_consume、memory_order_acquire、memory_order_release、memory_order_acq_rel、memory_order_seq_sct