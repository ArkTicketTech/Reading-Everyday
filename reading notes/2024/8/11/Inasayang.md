```
//固定长度
uint[8] array1;
bytes1[5] array2;
address[100] array3;
//可变长度
uint[] array4;
bytes1[] array5;
address[] array6;
```

bytes比较特殊，是数组，但是不用加[]。另外，不能用byte[]声明单字节数组，可以使用bytes或bytes1[]。bytes 比 bytes1[] 省gas

```
// memory动态数组
uint[] memory array8 = new uint[](5);
bytes memory array9 = new bytes(9);
```

声明后长度不能改变


结构体赋值

- 在函数中创建一个storage的struct引用
  ```
  function initStudent1() external{
    Student storage _student = student; // assign a copy of student
    _student.id = 11;
    _student.score = 100;
}
  ```
- 直接引用状态变量的struct
  ```
  function initStudent2() external{
    student.id = 1;
    student.score = 80;
  }
  ```
- 构造函数式
  ```
  function initStudent3() external {
    student = Student(3, 90);
  }
  ```
- key value
  ```
  function initStudent4() external {
    student = Student({id: 4, score: 60});
  }
  ```

```
mapping(uint => address) public idToAddress; // id映射到地址
mapping(address => address) public swapPair; // 币对的映射，地址到地址
```

- 映射的_KeyType只能选择Solidity内置的值类型
- 存储位置必须是storage

delete a会让变量a的值变为初始值
```
// delete操作符
bool public _bool2 = true; 
function d() external {
    delete _bool2; // delete 会让_bool2变为默认值，false
}
```

- 数值变量可以声明constant和immutable
- string和bytes可以声明为constant，但不能为immutable