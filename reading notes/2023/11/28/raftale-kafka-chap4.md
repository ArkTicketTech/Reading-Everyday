如何退出
如何安全干净的退出？
如果要关闭消费者并且希望立即退出，即使消费者可能正在等待长时间的poll，可以通过另一个线程调用consumer.wakeup()。如果你在主线程中运行消费者循环，则可以从shutdownHook执行此操作。
consumer.wakeup() 是唯一的可以安全地从不同线程调用的消费者方法。
调用wakeup()将导致poll()退出并抛出WakeupException，WakeupException不需要被处理，但在退出线程之前，必须调用consumer.close()。
代码：
https://github.com/gwenshap/kafka-examples/blob/master/SimpleMovingAvg/src/main/java/com/shapira/examples/newconsumer/simplemovingavg/SimpleMovingAvgNewConsumer.java

1. ShutdownHook 运行在一个独立的线程中，利用wakeup打断消费者poll
2. 如果poll loop 足够短，可以用原子变量在每个迭代中检测。
