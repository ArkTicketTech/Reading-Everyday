分布式锁的本质就是全局锁，使用redis实现一般是使用setnx原子命令.
```bash
> setnx lock:keyname true
1
> get lock:keyname
"true"
> setnx lock:keyname true
0
> del lock:keyname
```
```bash
锁过期：
> expire lock:keyname 10
1
# 10s后再执行
> get lock:keyname
nil
```

但setnx执行后可能出现错误，导致expire没有执行或者执行失败，该key就永远无法过期了。
原子命令：
```bash
set lock:keyname true ex 10 nx
```
