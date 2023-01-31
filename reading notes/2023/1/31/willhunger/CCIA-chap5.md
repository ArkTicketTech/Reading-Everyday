#### 5.2.5 标准原子整形的相关操作
* 共有操作：load()，store()，exchange()，compare_exchange_weak()，compare_exchange_strong()
* std::atomic<int> 和 std::atomic<unsigned long long> 具有 fetch_add()，fetch_sub()，fetch_or()，fetch_xor()，以及支持复合赋值（+=，-=，&=，!=，^=），以及自增操作；缺失了乘除法和移位操作。
* 函数原子化操作返回旧值，复合赋值运算新值，自增操作保持原来行为。