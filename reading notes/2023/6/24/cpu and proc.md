
| 函数 | 描述 |
| --- | --- |
| mycpu (kernel/proc.c:72) | 返回指向当前CPU结构的指针。RISC-V给每个CPU编号，称为hartid。xv6确保每个CPU的hartid存储在内核中的CPU的tp寄存器中。这允许mycpu使用tp索引一个cpu结构的数组以找到正确的cpu。 |
| mstart | 在CPU的启动序列的早期设置tp寄存器，此时还在机器模式中（kernel/start.c:51）。 |
| usertrapret | 在跳板页中保存tp，因为用户进程可能会修改tp。 |
| uservec | 当从用户空间进入内核时，恢复之前保存的tp（kernel/trampoline.S:70）。 |
| cpuid | 返回值是脆弱的：如果定时器中断并导致线程让步并移动到不同的CPU，以前返回的值将不再正确。为避免这个问题，xv6要求调用者禁用中断，并在使用返回的cpu结构体后再启用它们。 |
| myproc (kernel/proc.c:80) | 返回正在当前CPU上运行的进程的proc结构指针。myproc禁用中断，调用mycpu，从cpu结构中获取当前进程指针（c->proc），然后启用中断。即使中断被启用，myproc的返回值也是安全的：如果定时器中断将调用进程移动到不同的CPU，其proc结构指针将保持不变。 |
