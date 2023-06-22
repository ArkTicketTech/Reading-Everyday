

-   通过反射列出一个类型的所有方法和一个结构体类型的所有（导出和非导出）字段的类型

-   通过反射列出一个函数类型的各个输入参数和返回结果类型
    -   对于非接口类型，`reflect.Type.NumMethod`方法只返回一个类型的所有导出的方法（包括通过内嵌得来的隐式方法）的个数，并且 方法`reflect.Type.MethodByName`不能用来获取一个类型的非导出方法;对于接口类型，则并无这些限制
    -   虽然`reflect.Type.NumField`方法返回一个结构体类型的所有字段（包括非导出字段）的数目，但是不推荐。使用方法`reflect.Type.FieldByName`来获取非导出字段



可以通过反射来检视结构体字段的标签信息

-   结构体字段标签的类型为`reflect.StructTag`，它的方法`Get`和`Lookup`用来检视字段标签中的键值对
    -   键值对中的键不能包含空格（Unicode值为32）、双引号（Unicode值为34）和冒号（Unicode值为58）
    -   键值对形式中的冒号的后面不能紧跟着空格字符。所以`optional: "yes"`不形成键值对
    -   键值对中的值中的空格不会被忽略。所以`json:"author, omitempty“`、`json:" author,omitempty“`以及`json:"author,omitempty“`各不相同



无法通过反射动态创建一个接口类型

无法通过反射来声明一个新的类型



Pp. 289-293