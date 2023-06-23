`reflect.Value`类型和值

-   可以通过调用`reflect.ValueOf`函数，从一个非接口类型的值创建一个`reflect.Value`值
-   被一个`reflect.Value`值代表着的值常称为此`reflect.Value`值的底层值（`underlying value`）
-   `reflect.Value`值的`CanSet`方法将返回此`reflect.Value`值代表的Go值是否可以被修改（可以调用对应的
    `reflect.Value`值的`Set`方法来修改此Go值）
-   `reflect.ValueOf`函数直接返回的`reflect.Value`值都是不可修改的
-   `reflect.Value.Elem`方法也可以用来获取一个代表着一个接口值的动态值的`reflect.Value`值
-   如果一个`reflect.Value`值的底层值为一个函数值，则可以调用此`reflect.Value`值的Call方法来调用此函数



Pp. 293-301