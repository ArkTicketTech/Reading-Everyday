# trap.c

## devintr()

|步骤|操作|说明|
|---|---|---|
|1|获取scause的值|执行`uint64 scause = r_scause();`|
|2|检查是否是超级用户外部中断|`if((scause & 0x8000000000000000L) && (scause & 0xff) == 9)`|
|2.1|如果是，获取中断号irq|执行`int irq = plic_claim();`|
|2.2|根据irq调用相应设备的中断处理函数|`uartintr()`（如果irq是UART0的），`virtio_disk_intr()`（如果irq是VIRTIO0的）|
|2.3|打印错误信息|如果irq号不符合预期，打印错误信息|
|2.4|通知PLIC设备可以接收新的中断|处理完当前的中断后，调用`plic_complete(irq)`|
|2.5|返回1|代表其他设备的中断|
|3|检查是否是机器模式定时器中断的软件中断|`else if(scause == 0x8000000000000001L)`|
|3.1|如果是，处理时钟中断|如果`cpuid() == 0`，调用`clockintr()`函数|
|3.2|确认软件中断|通过清除`sip`寄存器中的SSIP位来确认软件中断|
|3.3|返回2|代表定时器中断|
|4|其他情况|如果不满足上述两种情况|
|4.1|返回0|表示中断未被识别|

1. `uartintr()`: 这个函数用于处理UART（通用异步收发器）设备的中断。UART是一种常见的串行通信协议，经常用于不同设备之间的数据交换。例如，计算机和其外设（如鼠标、键盘等）之间的通信，就可能使用UART协议。当UART设备有新的数据到来，或者数据已经发送完毕时，它会通过中断通知CPU。`uartintr()`函数就是用来处理这些中断的。

2. `virtio_disk_intr()`: 这个函数用于处理VirtIO硬盘设备的中断。VirtIO是一种开放标准，用于实现虚拟环境中的设备。它允许虚拟设备和主机之间的高效通信。当VirtIO硬盘设备完成一个读或写操作时，它会通过中断通知CPU。`virtio_disk_intr()`函数就是用来处理这些中断的。

# console.c

## consoleinit(void)
```c
void consoleinit(void)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}

```


## consoleread(int user_dst, uint64 dst, int n)
#### either_copyout()
- `either_copyout(user_dst, dst, &cbuf, 1)` 是一个用来复制数据的函数，它将数据从内核空间复制到用户空间或者另一个内核空间。它在内核中被广泛使用，因为内核需要经常地在用户空间和内核空间之间复制数据。

函数的参数说明如下：

- `user_dst`: 这是一个标志，它表示目标地址 `dst` 是在用户空间还是内核空间。如果 `user_dst` 是 `1`，则 `dst` 是用户空间地址；否则，`dst` 是内核空间地址。
  
- `dst`: 这是目标地址，即数据需要被复制到的位置。

- `&cbuf`: 这是源地址，即数据从哪里被复制。

- `1`: 这是要复制的字节数。

在这个特定的上下文中，函数 `either_copyout(user_dst, dst, &cbuf, 1)` 被用来将输入字节（在 `cbuf` 中）复制到用户空间的缓冲区（地址为 `dst`）。

#### 代码
```c
int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;
  int c;
  char cbuf;

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons.buffer.
    while(cons.r == cons.w){
      if(killed(myproc())){
        release(&cons.lock);
        return -1;
      }
      //第一个变量是唤醒条件，第二个是释放的锁
      sleep(&cons.r, &cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  return target - n;
}
```



## consoleintr(int c)
```c
void
consoleintr(int c)
{
  acquire(&cons.lock);

  switch(c){
  case C('P'):  // Print process list.
    procdump();
    break;
  case C('U'):  // Kill line.
    while(cons.e != cons.w &&
          cons.buf[(cons.e-1) % INPUT_BUF_SIZE] != '\n'){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): // Backspace
  case '\x7f': // Delete key
    if(cons.e != cons.w){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    if(c != 0 && cons.e-cons.r < INPUT_BUF_SIZE){
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

      if(c == '\n' || c == C('D') || cons.e-cons.r == INPUT_BUF_SIZE){
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    break;
  }
  
  release(&cons.lock);
}
```
## con结构体
```c
struct {
  struct spinlock lock;
  
  // input
  #define INPUT_BUF_SIZE 128
  char buf[INPUT_BUF_SIZE];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} cons;
```
# uart.c
## uartinit(void)
```c
void
uartinit(void)
{
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, LCR_BAUD_LATCH);
  
/*具体来说，在给定的寄存器架构中，将0x03写入0号寄存器（LSB）表示设置波特率的低位字节为0x03，将0x00写入1号寄存器（MSB）表示设置波特率的高位字节为0x00。这样的组合将波特率设置为38.4K，即每秒传输38,400个比特。*/
  // LSB for baud rate of 38.4K.
  WriteReg(0, 0x03);

  // MSB for baud rate of 38.4K.
  WriteReg(1, 0x00);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, LCR_EIGHT_BITS);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initlock(&uart_tx_lock, "uart");
}
```

|  代码   |                  作用                   |
|---------|---------------------------------------|
| WriteReg(IER, 0x00); | 禁用UART中断。 |
| WriteReg(LCR, LCR_BAUD_LATCH); | 设置特殊模式以设置波特率。 |
| WriteReg(0, 0x03); | 设置波特率的LSB为0x03。 |
| WriteReg(1, 0x00); | 设置波特率的MSB为0x00。 |
| WriteReg(LCR, LCR_EIGHT_BITS); | 退出波特率设置模式，设置数据字长为8位，无校验位。 |
| WriteReg(FCR, FCR_FIFO_ENABLE \| FCR_FIFO_CLEAR); | 复位并启用FIFO缓冲区。 |
| WriteReg(IER, IER_TX_ENABLE \| IER_RX_ENABLE); | 启用传输和接收中断。 |
| initlock(&uart_tx_lock, "uart"); | 初始化锁，用于保护对UART传输的访问。 |


## uartintr(void)
```c
void
uartintr(void)
{
  // read and process incoming characters.
  while(1){
    int c = uartgetc();
    if(c == -1)
      break;
    consoleintr(c);
  }

  // send buffered characters.
  acquire(&uart_tx_lock);
  uartstart();
  release(&uart_tx_lock);
}
```
## uartgetc(void)
```c
int
uartgetc(void)
{
  if(ReadReg(LSR) & 0x01){
    // input data is ready.
    return ReadReg(RHR);
  } else {
    return -1;
  }
}
```
# init.c
## main()
```c
int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  //dup复制文件描述符，连续两次是将标准输出和标准输入都重定向到一个终端中去
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
```
