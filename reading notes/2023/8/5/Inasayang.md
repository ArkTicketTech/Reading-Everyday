`reflect.Value.Bytes()`方法返回一个[]byte值，它的元素类型byte可能并非属主参数代表的Go切片值的元素类型

```
var mybs = []MyByte{'a', 'b', 'c'}
var bs []byte
// bs = []byte(mybs) // this line fails to compile
v := reflect.ValueOf(mybs)
bs = v.Bytes() // okay. Violating Go type system.
fmt.Println(bytes.HasPrefix(bs, []byte{'a', 'b'})) // true
bs[1], bs[2] = 'r', 't'
fmt.Printf("%s \n", mybs) // art
```



应该使用`os.IsNotExist(err)`而不是`err== os.ErrNotExist`来检查文件是否存在

-   使用`err == os.ErrNotExist`可能漏掉一些错误

```
_, err := os.Stat("a-nonexistent-file.abcxyz")
fmt.Println(os.IsNotExist(err)) // true
fmt.Println(err == os.ErrNotExist) // false
```

-   使用`errors.Is(err,os.ErrNotExist)`来检查文件是否存在

```
_, err := os.Stat("a-nonexistent-file.abcxyz")
fmt.Println(errors.Is(err, os.ErrNotExist)) // true
```



flag标准库包对待布尔命令选项不同于数值和字符串选项

-   传递程序选项有三种形式
    -   -flag：仅适用于布尔选项
    -   -flag=x：用于任何类型的选项
    -   -flag x：仅用于非布尔选项
-   使用第一种形式的布尔选项将被视为最后一个选项，其后面的所有项都被视为参数



`[Sp|Fp|P]rintf`函数支持位置参数

```
// The next line prints: coco
fmt.Printf("%[2]v%[1]v%[2]v%[1]v", "o", "c")
```



Pp. 546-549