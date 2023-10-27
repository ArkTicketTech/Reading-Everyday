DTO: Data Transfer Object

DMO: Data Model Obejct



### 3种让线程等待和唤醒的方法

1. Object中的wait()方法让线程等待，notify()方法唤醒线程
2. JUC包中Condition的await()方法让线程等待，signal()方法唤醒线程
3. LockSupport类中的park()阻塞线程，unpark()解除阻塞线程