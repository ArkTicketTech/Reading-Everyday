```
function returnMultiple() public pure returns(uint256, bool, uint256[3] memory){
    return(1, true, [uint256(1),2,5]);
}
```

数组类型返回值默认必须用memory修饰


解构赋值
```
(_number, _bool, _array) = returnNamed();
(, _bool2, ) = returnNamed();
```

引用类型需要声明数据存储的位置，不同存储位置的gas成本不同

- storage
  - 合约里的状态变量默认存储位置
  - 上链
  - 类似计算机的硬盘，消耗gas多
- memory
  - 函数里的参数和临时变量一般用memory
  - 存储在内存中
  - 不上链
  - 消耗gas少
- calldata
  - 与memor类似
  - 变量不能修改，一般用于函数参数

```
function fCalldata(uint[] calldata _x) public pure returns(uint[] calldata){
    //参数为calldata数组，不能被修改
    // _x[0] = 0 //这样修改会报错
    return(_x);
}
```

三种变量：

 - 状态变量（state variable）
   - 数据存储在链上的变量
   - 在合约内、函数外声明
 - 局部变量（local variable）
   - 数据存储在内存里
   - 在函数内声明
 - 全局变量(global variable)
   - solidity预留关键字
   - 不声明直接使用
   - https://learnblockchain.cn/docs/solidity/units-and-global-variables.html#special-variables-and-functions
