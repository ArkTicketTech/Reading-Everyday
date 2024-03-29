
| 难题 | 解决方案 |
| --- | --- |
| 如何从一个进程切换到另一个进程？ | 利用上下文切换的概念。尽管实现起来复杂，但这是xv6中最不透明的代码部分。 |
| 如何强制进行切换以使用户进程透明？ | xv6使用标准技术，其中硬件计时器的中断驱动上下文切换。 |
| 所有的CPU在相同的共享进程集中进行切换，如何避免竞争？ | 需要一个锁定计划来避免竞态条件。 |
| 当进程退出时，必须释放进程的内存和其他资源，但进程不能自行完成所有这些操作，如何处理？ | 因为进程不能在使用内核栈的同时释放它，所以需要特别的处理方法。 |
| 多核机器的每个核心必须记住它正在执行哪个进程，以便系统调用影响正确的进程的内核状态，如何处理？ | 这需要精确地管理每个核心正在执行的进程状态。 |
| sleep和wakeup允许进程放弃CPU并等待被另一个进程或中断唤醒，如何避免由此产生的竞态条件？ | 需要小心地管理，以避免丢失唤醒通知的竞态条件。xv6尝试尽可能简单地解决这些问题，尽管结果的代码是复杂的。 |

![](file:///C:\Users\BBD\AppData\Roaming\Tencent\Users\2513209473\QQ\WinTemp\RichOle\YSXS0LUMPWU920WYNLRZ_AX.png)