分布式系统，每台机器都有自己的时钟，我们如何保证各台机器的时间相同？如何考虑网络延迟对事件先后到达的影响？

时钟同步也是一种分布式系统，本质上还是机器之间通过网络传递消息来完成任务。

# 物理时钟

1. 基于钟摆原理的机械时钟
2. 基于石英晶体的石英时钟
3. 基于原子共振的原子钟



时间标准：

1. UTC
2. 时间戳



计算机使用的是石英钟，并通过时钟同步不时地调整时钟来纠正误差，最常见的时钟同步方法是使用NTP。

# 时钟同步

Network Time Protocol

NTP是一个时钟同步的网络协议。

NTP通常可以在公共物联网保持几十毫秒的误差，在局域网环境中误差通常小于1毫秒。



开发时可能遇到时钟回退问题：

```java
long startTime = System.currentTimeMillis();
doSomething(); // 如果此时NTP正好进行同步
long endTime = System.currentTimeMillis();
long estimatedTime = endTime - startTime; // 结果可能为负数
```

如果要解决这个问题，需要使用单调时钟，Java中System.nanoTime()就是一个单调时钟。

```java
long startTime = System.currentTimeMillis();
doSomething(); 
long endTime = System.currentTimeMillis();
long estimatedTime = endTime - startTime; // 结果一定大于0
```

但这种单调时钟只在单机系统中，分布式中的单调时钟是下面将要讲到的逻辑时钟。

# 逻辑时钟

Happens before关系

1. 全序关系：全部元素都可以进行比较
2. 偏序关系：部分元素才可以进行比较，即部分元素有序



# 向量时钟



# 分布式快照
