`sync.Cond`类型

-   每个`sync.Cond`值拥有一个`sync.Locker`类型的名为L的字段

-   `*sync.Cond`类型有三个方法：`Wait()`、`Signal()`和`Broadcast()`

-   每个`Cond`值维护着一个先进先出等待协程队列

-   如果需要，多个`sync.Cond`值可以共享一个`sync.Locker`值。但是这种情形在实践中并不多见

    ```
    cond := sync.NewCond(&sync.Mutex{})
    ```

    



Pp. 421