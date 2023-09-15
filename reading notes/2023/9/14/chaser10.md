### Slice
##### Q：数组与切片的区别
- 数组是值类型 长度固定 长度是类型的一部分
- 切片是引用类型 是一个维护了底层数组指针、数组容量以及切片长度的结构体指针 其长度是可变的 当长度超过了底层数组容量会进行扩容
##### Q：切片的扩容机制
- 当切片的长度超过了底层数组的容量时 切片会发生扩容 不同版本的Go其扩容机制不同
- Go 1.21的扩容规则为 当新的切片长度大于旧容量的两倍 那么新容量为新切片长度  如果不满足此条件继续判断slice的长度是否大于256 小于256时新容量将两倍扩增 如果大于256则近似扩增1.25倍  此外最后的容量还得向上对齐到8的倍数  确定好容量之后会将原来底层数组的内容整体搬迁到新的数组 时间复杂度是**O(n)**

### Map

##### Q：map的底层实现
- map的底层结构体是hmap 维护了哈希种子 哈希桶数组的大小以及其数组数组指针，进行键值操作时 实现会先计算key‘的哈希值 其中**低位**决定key将落入哪个bucket中 而**高位**决定key将落入bucket中哪个cell中 其中key和value在bucket中是分开存储的是为了很好的利用内存 假如发生哈希碰撞 将把这个键值对放入overflow bucket当中（就是拉链法解决哈希碰撞）
##### Q：map的扩容机制
- 增量扩容 增量扩容发生在**装载因子（元素数量/哈希表大小）> 6.5**时 此时哈希表的容量扩大到原先的两倍 然后对元素进行rehash使其落入不同桶内(原来是hash值低5位用来寻找落入哪个桶 扩容后低6位用来寻找落入哪个桶
- 等量扩容 发生在map频繁增删导致bucket以及overflow bucket桶中元素非常稀疏会十分降低查找效率时 将overflow bucket中元素搬迁到bucket中使bucket中元素变得紧密
- 搬迁不是一蹴而就的 因为如果存在大量数据 一次搬迁要花费太长时间 所以他在map每次进行操作时搬迁一到两个bucket
##### Q：map做并发操作有什么问题
- map出于性能考量以及其显式的设计哲学(语言的并发支持应该是显式的而不是默认语言会帮你处理)不支持并发，map底层是维护了一个标志位的如果标志位的写标志为1时代表有协程正在写 在进行kv操作时会检查这个标志位如果为1就会直接panic
##### Q：有了解什么机制或者工具可以检测自己是否存在map的并发读写情况吗？
- go build或者go test添加 -race选项
- pprof
##### Q：sync.Map是怎么实现的

- [源码解读 Golang 的 sync.Map 实现原理 - 掘金 (juejin.cn)](https://juejin.cn/post/6844904100287496206)
- [Golang sync.Map 原理（两个map实现 读写分离、适用读多写少场景）-阿里云开发者社区 (aliyun.com)](https://developer.aliyun.com/article/1172753)

- sync.Map 是典型的读写分离策略

- sync.Map中维护了一个dirty map和一个read only其中read only map是dirty map的子集

- 当读取时会先查询read 这个read因为是只读的所以并发安全（**相当于dirty map的缓存**）  不存在时再上锁（**上锁的间隙中要注意dirty map可能赋给了read map 所以要double chec**k）查询dirty 然后查询到的结果会缓存至read only map并且会将miss计数加1 当miss达到阈值时 会将dirty map赋给read map     

- 写操作时只将数据写入dirty map(**这里要注意缓存一致性的问题  在源码中read map缓存的是key对应的value的指针 所以如果有此key的缓存 直接更新 read map中获取的value指针值  这里因为是不线程安全的操作所以源码中采用了CAS原子操作**) 写操作先查询read中是否有 如果有dirty map中一定有 直接使用**CAS操作**更新read map中value指针值 若没有就加锁写入到dirty map中

- 删除操作 若read中有则表示dirty中一定有 直接用**CAS操作**将value值更新为nil  若没有加锁更新dirty map

- 读多写少的情况 read map的命中率高的情况下 比较合适  如果写多 那么也会频繁的上锁去写dirty map以及交换dirty map read map的情况  不如分片锁+map

### GMP
##### Q：协程调度是什么样的
- 协程是用户级线程**是一个逻辑上的线程**对于操作系统而言是不可见的，不受操作系统的调度和管理，而**线程是操作系统运行调度的最小单位**。

- 所以如果协程想要运行就得将他映射到一个内核级的线程中运行  如果M个协程就有M个线程的话那么将会产生大量的上下文切换 或者占用的大量的内存 如果M个协程以及一个线程的话 假如发生阻塞那么除非他自己放弃cpu时间片 否则会一直占有 所以就有了折中的M:N模型  GMP模型要做的事情就是将M个goroutine调度到N个线程上执行       

- 在GMP调度模型当中 G是goroutine M是内核级线程 是能够被操作系统调度到cpu执行的最小单位  P是逻辑处理器

- 每一个p包括了一个本地Goroutine队列  M需要绑定一个P才能从中获取goroutine进行执行（**所以可以同时执行的G和P的数量有关**）  当绑定了一个p后就能从本地的goroutine队列中获取goroutine执行 如果本地队列也没有 则会去其他队列中偷取一半的goroutine过来执行    

- 当G发生阻塞了 那么P将会与这个M解绑  P将寻找空闲的M或者创建一个新的M接管（**如果是channel阻塞 那么仅仅是Goroutine被挂起 挂起信息会被维护在channel当中 当channel可读可写时将会唤醒goroutine M会去尝试获取P  如果是系统调用阻塞 那么是一块被挂起**）

### String
##### Q：如何将string转为[]byte，不申请额外空间
- string 底层是（byte数组指针（data）,len）二元组数据结构的引用
- 可以将任何类型的指针类型转换成unsafe.pointer 也可以将unsafe.pointer转换成任意类型的指针类型
- 首先需要获取string底层的byte数组指针
	- 首先将string转换成unsafe.Pointer类型再转换成reflect.StringHeader指针类型 此时就可以通过StringHeader拿到data指针(新版本已弃用reflect.StringHeader)
	- `unsafe func StringData(str string) *byte`：返回指向该 string 的字节数组的数据指针
- 拿到byte数组指针接着就是构造SliceHeader(data，len，cap)三元组
	- 通过reflect.SliceHeader构造SliceHeader类型的指针然后将之转换成unsafe.Pointer指针 然后再转换成[]byte类型的指针最后再解引用就得到了[]byte(新版本已弃用reflect.SliceHeader)
	- `unsafe func Slice(ptr *ArbitraryType, len IntegerType) []ArbitraryType`接收任意类型的指针以及长度返回一个任意类型的切片


### 并发
##### Q：channel是怎么实现的
- channel的底层数据结构是hchan 其中最主要维护了环形数组用于缓存有缓冲channel的数据 以及两个阻塞的goroutine队列 注意这个结构体是带锁的
- 无缓冲的channel
	- 当尝试往channel写数据时 如果channel为nil则会直接挂起goroutine 如果channel关闭了则会panic 如果recvx队列中有阻塞的goroutine就把写入值拷贝给这个goroutine并唤醒他 如果recvx为空 则将此goroutine挂起 并加入到sendx队列中
	- 当尝试往channel中读数据时 如果channel为nil则会直接挂起 如果channel关闭了直接返回 如果sendx中有发送方阻塞则唤醒并拷贝数据 如果没有则挂起加入到recvx
- 有缓冲的channel
	- 相比于无缓冲的 多了一步判断缓冲区 如果没有接收方阻塞 就判断缓冲区是否满了  满了则挂起阻塞 没满拷贝至缓冲区
	- 接收时如果有阻塞发送方并且缓冲区有数据则拷贝缓冲区数据并唤醒 如果缓冲区有数据但没有阻塞的发送方那就简单的拷贝缓冲区数据  如果都没有的话 就挂起

##### Q：设计一个并发安全的计数器
- atomic包 简单的问题会快一点
- mutex锁 涉及到复杂场景 多个值存在数据竞争
- channel


### 框架
##### Q：gin框架的路由数据结构是怎么实现的
- radix tree(压缩前缀树) 前缀树的变种 当子节点层数小于2就可以合并到一起
- 节省空间 不用为一个字符就分配一个节点  查找速度快 并且很灵活  支持动态路由支持通配符
##### Q：go官方的路由是用什么数据结构实现的
- 官方的路由是通过map实现的
- 每次路由一个路径将会遍历一遍所有注册的路由函数 找到最长路径前缀

### 网络编程
##### Q：Go的网络io多路复用模型，数据结构
[Go netpoller 原生网络模型之源码全面揭秘 - Strike Freedom](https://strikefreedom.top/archives/go-netpoll-io-multiplexing-reactor#toc-head-18)
- Go的netpoller本质是epoll+非阻塞io
- 所有网络操作底层抽象为对netFD的操作
- **netFD**是对**poll.fd**的封装 poll.fd封装了一个真正的socket fd和**pollDesc**（事件驱动的封装）
- 当初始化listener时底层会初始化epoll并将listen fd与polldesc作为epoll event参数中的data 加入到epoll当中
- 当调用netFD（listener底层）的accept，**netFd会去调用poll.fd的accept如果遇到错误(非阻塞io 遇到错误则是事件没有就绪)就会将当前goroutine存储到这个netFD对应的pollDesc中**，并调用gopark阻塞 将goroutine放入阻塞队列。
- 当go scheduler或者监控线程调用netpoll会找到事件就绪的goroutine 并从polldesc中取出goroutine信息并将其唤醒
- netpoll底层就是epollwait

##### Q：go中如果将listener关闭 那么之前的accept连接是否会关闭？
- 无论在哪个语言中都不会的 因为accept获取连接 返回的是一个套接字 关闭的只是listener套接字


### 文件io
##### Q：go中如何判断一个文件读结束了
- 读结束会返回一个io.EOF错误
##### Q：go中如何在打开一次文件后，读到末尾，不重新打开文件，再读一次？
- seek方法重置设定位置


### Context
##### Q：Context有什么用，怎么用，包含什么
- context主要用于树状结构的goroutine之间传递信号
- 声明一个context需要指定他的parent context 一切的context都应该从context.Background中衍生
- context接口主要包括了四个方法
	- Done 返回一个channel 表示context被取消的信号
	- Err 返回一个错误，表示channel被关闭的原因
	- Deadline 返回context的截至时间
	- Value() 获取之前设置的key对应的value
- 子协程使用select阻塞done channel当父协程调用cancel函数 将会关闭channel 并会通知所有子协程调用cancel

### GC
##### Q：GC过程
- 起初所有对象为白色（go 1.8所有栈对象一开始就为黑色）
- 从根对象出发所有根对象可达的对象被标记为灰色
- 遍历所有灰色对象将灰色对象的可达对象也标为灰色
- 当标记完一个灰色对象的所有可达对象后 将灰色对象标为黑色
- 最后清除所有白色对象
- 为了保护在并发gc中不会错误的清除 go1.8中引入了混合写屏障 被删除的对象以及被引用的对象都会被标为灰色  假如说真的是要被清除的将会在下一轮gc中清除
##### Q：GC中的根对象是什么
- 全局变量 函数调用栈中的局部变量

##### Q：golang触发GC的时候，在GC期间产生的垃圾、需要回收的对象怎么处理？
- 将要回收对象的所占用内存加入到空闲列表中，那么这块内存空间就可以在后续的内存重新分配