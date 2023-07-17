`sync/atomic`标准库包中提供的原子操作

-   原子操作是无锁的，常常直接通过CPU指令直接实现



整数原子操作

-   `StoreT`和`LoadT`原子操作函数或者方法经常被用来需要并发运行的实现`setter`和`getter`方法

    ```
    type Page struct {
    	views atomic.Uint32
    }
    func (page *Page) SetViews(n uint32) {
    	page.views.Store(n)
    }
    func (page *Page) Views() uint32 {
    	return page.views.Load()
    }
    ```



-   如果`T`是一个有符号整数类型，比如`int32`或`int64`，则`AddT`函数调用的第二个实参可以是一个负数，用来实现原子减法操作
-   如果`T`是一个无符号整数类型，比如`uint32`、`uint64`或者`uintptr`，则`AddT`函数调用的第二个实参需要为一个非负数
    -   第二个实参为类型为T的一个变量值v
        -   因为-v在Go中是合法的，所以-v可以直接被用做AddT调用的第二个实参
    -   第二个实参为一个正整数常量c
        -   可以使用`^T(c-1)`（仍为一个正数）做为AddT调用的第二个实参
        -   此`^T(v-1)`小技巧对于无符号类型的变量v也是适用的，但是`^T(v-1)`比`T(-v)`的效率要低
        -   对于这个`^T(c-1)`小技巧，如果c是一个类型确定值并且它的类型确实就是T，则它的表示形式可以简化为`^(c-1)`



Pp. 425-432