##### STORING A NEW VALUE (OR NOT) DEPENDING ON THE CURRENT VALUE
compare-exchange 操作：具有 compare_exchange_weak()和 compare_exchange_strong() 两种形式。其对原子类型的当前值和期望值进行比较：如果两值相等时，存储所提供值，否则期望值会被更新为当前原子变量中的值。

compare_exchange_weak() 可能会导致伪失败（spurious failure），这种情况下变量的值不会发生改变，并且 compare_exchange_weak() 返回 false。这种情况可能发生在缺少单条 CAS 指令的机器上，由于当前线程可能被操作系统切换调度，处理器并不能保证这个操作能原子完成。

为了避免伪失败，compare_exchange_weak() 通常配合循环使用：
```
bool expected=false;  
extern atomic<bool> b; // set somewhere else while(!b.compare_exchange_weak(expected,true) && !expected);
```
