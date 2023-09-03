#### CSP
**不要通过共享内存来通信，而要通过通信来实现内存共享**
1. 每个进程可以并发执行**只能通过通信来影响其他进程**
2. 进程唯一的交互方式只有点到点的消息传递机制 不存在共享内存
3. 同步通信 通信是同步的只有双方都准备好了才能执行通信操作
CSP是并发程序的设计模型，把进程通信放在首要位置，简化并发编程问题

#### 数据结构
channel底层是指向hchan的指针

```
type hchan struct {
    // chan 里元素数量
    qcount   uint
    // chan 底层循环数组的长度
    dataqsiz uint
    // 指向底层循环数组的指针
    // 只针对有缓冲的 channel
    buf      unsafe.Pointer
    // chan 中元素大小
    elemsize uint16
    // chan 是否被关闭的标志
    closed   uint32
    // chan 中元素类型
    elemtype *_type // element type
    // 已发送元素在循环数组中的索引
    sendx    uint   // send index
    // 已接收元素在循环数组中的索引
    recvx    uint   // receive index
    // 等待接收的 goroutine 队列
    recvq    waitq  // list of recv waiters
    // 等待发送的 goroutine 队列
    sendq    waitq  // list of send waiters

    // 保护 hchan 中所有字段
    lock mutex
}
```
1. buf底层是循环数组，用于存储缓冲数据，只有带缓冲的channel才有
2. sendx和recvx表示可以发送和接收的位置索引
3. sendq，recvq表示被阻塞的goroutine  底层是一个双向链表

#### 发送数据
1. 判断channel是不是nil是的话就挂起
2. 接受队列中有goroutine，唤醒阻塞的第一个goroutine，并把数据拷贝过去
3. 如果缓存channel的缓冲区还有剩余坑位就将他拷贝buf的sendx位置
4. 以上条件都不满足就加入到发送等待队列并挂起goroutine

#### 接收数据
1. 如果channel是nil直接挂起
2. 如果channel关闭了且缓冲区元素个数为0 直接返回
3. 如果发送队列里有阻塞的goroutine那么分成两种情况
	1. 无缓冲的 直接将发送gotoutine中数据拷贝到当前gouroutine 并唤醒
	2. 有缓冲的 接收循环数组的头部元素 并将发送队列头个数据放入循环数组尾部
4. 如果缓存区中有数据 直接拿缓冲区的数据
5. 缓冲区无数据 发送队列也没有阻塞goroutine 则将之加入到接收队列并挂起等待唤醒

#### 关闭channel
1. 如果channel是nil或者已关闭则panic
2. 唤醒所有等待队列的goroutine
3. 所有发送队列goroutine将会panic

| 操作      | nil channel | closed channel | not nil, not closed channel                           |
|---------|-------------|----------------|-------------------------------------------------------|
| close   | panic       | panic          | 正常关闭                                                  |
| 读 <- ch | 阻塞          | 读到对应类型的零值      | 阻塞或正常读取数据。缓冲型 channel 为空或非缓冲型 channel 没有等待发送者时会阻塞     |
| 写 ch <- | 阻塞          | panic          | 阻塞或正常写入数据。非缓冲型 channel 没有等待接收者或缓冲型 channel buf 满时会被阻塞 |

#### 优雅关闭channel
[如何优雅地关闭 channel? | Go优质外文翻译 | Go 技术论坛 (learnku.com)](https://learnku.com/go/t/23459/how-to-close-the-channel-gracefully)
使用channel时有几个痛点
1. 在不改变channel状态情况下，无法获知channel是否关闭  使用if ok 语句的话则会读出来一个数据
2. 关闭一个已关闭的channel会导致panic
3. 向一个已关闭的channel发送数据会panic

**不要从一个 receiver 侧关闭 channel，也不要在有多个 sender 时，关闭 channel。**

有两个不那么优雅关闭channel的办法
1. defer-recover
2. sync.Once

根据sender和receiver的个数分以下几种情况
1. 一个 sender，一个 receiver
2. 一个 sender， M 个 receiver
3. N 个 sender，一个 reciver
4. N 个 sender， M 个 receiver

一个sender情况直接从sender关
第3种情况     由receiver通知sender 关闭通道the only receiver says “please stop sending more” by closing an additional signal channel。

```golang
func main() {
	rand.Seed(time.Now().UnixNano())

	const Max = 100000
	const NumSenders = 1000

	dataCh := make(chan int, 100)
	stopCh := make(chan struct{})

	// senders
	for i := 0; i < NumSenders; i++ {
		go func() {
			for {
				select {
				case <- stopCh:
					return
				case dataCh <- rand.Intn(Max):
				}
			}
		}()
	}

	// the receiver
	go func() {
		for value := range dataCh {
			if value == Max-1 {
				fmt.Println("send stop signal to senders.")
				close(stopCh)
				return
			}

			fmt.Println(value)
		}
	}()

	select {
	case <- time.After(time.Hour):
	}
}
```

第4种情况 使用一个中间人关闭any one of them says “let’s end the game” by notifying a moderator to close an additional signal channel

```golang
func main() {
	rand.Seed(time.Now().UnixNano())

	const Max = 100000
	const NumReceivers = 10
	const NumSenders = 1000

	dataCh := make(chan int, 100)
	stopCh := make(chan struct{})

	// It must be a buffered channel.
	toStop := make(chan string, 1)

	var stoppedBy string

	// moderator
	go func() {
		stoppedBy = <-toStop
		close(stopCh)
	}()

	// senders
	for i := 0; i < NumSenders; i++ {
		go func(id string) {
			for {
				value := rand.Intn(Max)
				if value == 0 {
					select {
					case toStop <- "sender#" + id:
					default:
					}
					return
				}

				select {
				case <- stopCh:
					return
				case dataCh <- value:
				}
			}
		}(strconv.Itoa(i))
	}

	// receivers
	for i := 0; i < NumReceivers; i++ {
		go func(id string) {
			for {
				select {
				case <- stopCh:
					return
				case value := <-dataCh:
					if value == Max-1 {
						select {
						case toStop <- "receiver#" + id:
						default:
						}
						return
					}

					fmt.Println(value)
				}
			}
		}(strconv.Itoa(i))
	}

	select {
	case <- time.After(time.Hour):
	}

}
```

#### 本质
本质就是值拷贝无论是从sender goroutine到chan buf 还是chan buf到receiver goroutine

#### 泄露
channel处于等待或发送队列当中 goroutine处于挂起状态 并且得不到改变