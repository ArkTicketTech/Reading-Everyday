如何正确地使用非类型安全指针？（先略过）



泛型（略）



反射概述

-   通过reflect库包中Type和Value两个类型提供的功能来观察不同的Go值
-   反射机制设计的目标之一是任何非反射操作都可以通过反射机制来完成



`reflect.Type`类型和值

-   调用`reflect.TypeOf`函数，从一个任何非接口类型的值创建一个`reflect.Type`值
-   `reflect.Type`值表示此非接口值的类型
-   将一个接口值传递给一个`reflect.TypeOf`函数调用，将返回一个表示着此接口值的动态类型的`reflect.Type`值
-   `reflect.TypeOf`函数的唯一参数的类型为`interface{}`， 总是返回一个表示着此唯一接口参数值
    的动态类型的`reflect.Type`值

```
7| type A = [16]int16
8| var c <-chan map[A][]byte
9| tc := reflect.TypeOf(c)
10| fmt.Println(tc.Kind()) // chan
11| fmt.Println(tc.ChanDir()) // <-chan
12| tm := tc.Elem()
13| ta, tb := tm.Key(), tm.Elem()
14| fmt.Println(tm.Kind(), ta.Kind(), tb.Kind()) // map 
array slice
15| tx, ty := ta.Elem(), tb.Elem()
16|
17| // byte是uint8类型的别名。
18| fmt.Println(tx.Kind(), ty.Kind()) // int16 uint8
19| fmt.Println(tx.Bits(), ty.Bits()) // 16 8
20| fmt.Println(tx.ConvertibleTo(ty)) // true
21| fmt.Println(tb.ConvertibleTo(ta)) // false
```



Pp. 283-289