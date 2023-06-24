分号插入规则

-   大多数分号都是可选的，因此常常被省略

-   在编译时刻，Go编译器会自动插入这些省略的分号

-   不能在选择器中的句点.之前断行

    合法

    ```
    1| anObject.
    2| 	MethodA().
    3| 	MethodB().
    4| 	MethodC()
    ```

    非法

    ```
    1| anObject
    2| .MethodA()
    3| .MethodB()
    4| .MethodC()
    ```

    



Pp. 301-307