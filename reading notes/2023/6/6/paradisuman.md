## Kernelvec.S
| 功能模块 | 代码段 | 功能描述 |
|---|---|---|
| kernelvec | .globl kernelvec .align 4 kernelvec: ... sret | 这部分代码主要处理内核模式下的异常或中断。首先，保存所有寄存器的状态，然后调用`kerneltrap`处理异常或中断。处理完毕后，恢复寄存器状态，并通过`sret`返回到被中断的内核代码执行位置。|
| timervec | .globl timervec .align 4 timervec: ... mret | 这部分代码主要处理机器模式下的定时器中断。首先，保存a0，a1，a2，a3寄存器的值，然后根据预设的间隔计算下一次的定时器中断时间并设置CLINT的MTIMECMP寄存器。接下来，向sip寄存器写入2，表示将在这个处理程序返回后产生一个在特权级为S的软件中断。最后，恢复a1，a2，a3寄存器的值，并通过`mret`返回到被中断的执行位置。|

## trampoline.S
- 这段代码是 xv6 操作系统用于处理用户空间陷阱（trap）的低级别代码。这段代码有两部分：`uservec` 和 `userret`。
- `uservec` 是处理陷阱的代码，它保存了所有的用户寄存器，然后切换到内核的页表，并跳转到 `usertrap` 函数。

- `userret` 是从内核返回到用户空间的代码，它恢复了所有的用户寄存器，然后切换到用户的页表，最后返回到用户空间。

以下是这两部分的关键步骤：

1. **uservec**
   - 保存用户 `a0` 寄存器到 `sscratch`，以便后续使用 `a0`。
   - 设置 `a0` 到 `TRAPFRAME`，这是保存所有用户寄存器的地方。
   - 使用 `sd` 指令保存所有的用户寄存器到 `TRAPFRAME`。
   - 从 `TRAPFRAME` 加载内核栈指针、当前的 hartid、`usertrap` 函数的地址和内核页表地址。
   - 使用 `sfence.vma` 指令等待所有之前的内存操作完成。
   - 使用 `csrw` 指令安装内核页表，并使用 `sfence.vma` 刷新 TLB。
   - 跳转到 `usertrap` 函数。

2. **userret**
   - 接收用户页表地址作为参数。
   - 使用 `sfence.vma` 和 `csrw` 指令切换到用户页表，并刷新 TLB。
   - 设置 `a0` 到 `TRAPFRAME`。
   - 使用 `ld` 指令恢复所有的用户寄存器，除了 `a0`。
   - 恢复用户的 `a0` 寄存器。
   - 使用 `sret` 指令返回到用户空间。

这段代码实现了从用户空间到内核的切换以及从内核返回到用户空间的过程，这两个过程都需要保存和恢复所有的寄存器，并正确地切换页表。


## trap.c
1. `trapinit(void)`: 初始化一个称为 "tickslock" 的自旋锁。自旋锁是一种简单的同步机制，用于在多线程环境中保护共享资源。
2. `trapinithart(void)`: 这个函数用于将 `kernelvec` 设置为异常和中断的处理函数。这里的 `hart` 是 RISC-V 架构的术语，代表硬件线程。
3. `usertrap(void)`: 这个函数处理从用户空间产生的中断、异常或系统调用。如果发生系统调用，它会调用 `syscall()` 函数。如果是设备中断，它会调用 `devintr()` 函数。
4. `usertrapret(void)`: 这个函数用于从内核空间返回到用户空间。它设置了一些必要的寄存器，并且跳转到 `trampoline.S` 中的 `userret`。
5. `kerneltrap()`: 这个函数处理从内核空间产生的中断和异常。
6. `clockintr()`: 这个函数处理时钟中断，它会增加 `ticks` 计数，并唤醒任何等待 `ticks` 的进程。
7. `devintr()`: 这个函数检查是否发生了外部中断或软件中断，并进行处理。如果是时钟中断，返回值是 2，如果是其他设备中断，返回值是 1，如果无法识别的中断，返回值是 0。
其中，许多函数（如 `r_sstatus()`, `r_sepc()`, `r_scause()` 等）是用于读取 RISC-V 的特殊寄存器，而 `w_stvec()`, `w_sstatus()`, `w_sepc()` 等函数则用于写入这些寄存器。

-   PLIC（Platform-Level Interrupt Controller）是一个硬件模块，用于管理和分发中断信号。它是在RISC-V架构中广泛使用的中断控制器。