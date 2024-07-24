原生数据类型：

1. boolean
2. uint256: unsigned integer
3. int256: int256 ranges from -2 ** 255 to 2 ** 255 - 1
4. address



变量类型：

1. local: 局部变量，存储在函数中，不存储在区块链上
2. state：成员变量
3. global：全局变量

```solidity
contract Variable {
    // state
    string public text = "hello";
    uint256 public num = 123;

    function doSomething() public view returns(uint)  {
        // local
        uint256 i = 456;
        // global variables
        uint256 timestamp =  block.timestamp;
        address sender = msg.sender;
        return timestamp;
    }
}
```

# 常量

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

contract Constants {
    // coding convention to uppercase constant variables
    address public constant MY_ADDRESS =
        0x777788889999AaAAbBbbCcccddDdeeeEfFFfCcCc;
    uint256 public constant MY_UINT = 123;
}
```

# 不可变量

关键词：immutable

# 修改变量

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

contract SimpleStorage {
  // State variable to store a number
  uint256 public num;

  // You need to send a transaction to write to a state variable.
  function set(uint256 _num) public {
    num = _num;
  }

  // You can read from a state variable without sending a transaction.
  function get() public view returns (uint256) {
    return num;
  }
}
```

# Ether and Wei

以太坊的货币单位为「ether」，就是交易所的eth，eth的最小单位为wei，`1 eth = 10^18 wei`。

```
1Gwei = 10^9 wei
```

# Gas

gas是衡量执行交易所需计算资源。gas通常用`Gwei`来估量。

也就是主链上打包一个交易需要gas fee，gas fee包括base fee和max fee，gas fee越高，矿工越优先打包交易。



# if else / For / while

跟大多数语言差不多

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

contract Loop {
  function loop() public {
    // for loop
    for (uint256 i = 0; i < 10; i++) {
      if (i == 3) {
        // Skip to next iteration with continue
        continue;
      }
      if (i == 5) {
        // Exit loop with break
        break;
      }
    }

    // while loop
    uint256 j;
    while (j < 10) {
      j++;
    }
  }
}
```

# Mapping

相当于hashmap

```solidity
pragma solidity ^0.8.24;

contract Mapping {
  // Mapping from address to uint
  mapping(address => uint256) public myMap;

  function get(address _addr) public view returns (uint256) {
    // Mapping always returns a value.
    // If the value was never set, it will return the default value.
    return myMap[_addr];
  }

  function set(address _addr, uint256 _i) public {
    // Update the value at this address
    myMap[_addr] = _i;
  }

  function remove(address _addr) public {
    // Reset the value to the default value.
    delete myMap[_addr];
  }
}
```

# Array

数组，跟链表有点类似。

可以push, pop, delete

# Enum

枚举

# Structs

跟类很类似



# Data Locations - Storage, Memory and Calldata

变量分为值类型和引用类型。

引用类型如数组和结构体的数据存储位置可能为：

1. storage：以太坊中，每个特定的智能合约都有自己的存储，存储是所有合约状态变量所在的地方。生命周期同contract一致。
2. memory：生命周期存在于function内
3. calldata：存储函数参数，只读。外部函数的入参被强制制定为calldata，效果与memory差不多。



# Transient storage

一次交易执行完成之后，就自动销毁。

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity >=0.8.24;

contract Storage {
  modifier nonreentrant() {
    assembly {
      if tload(0) {
        revert(0, 0)
      }
      tstore(0, 1)
    }
    _;
    assembly {
      tstore(0, 0)
    }
  }
}
```

# 函数
函数调用分为两类：

1. 内部调用
2. 外部调用

## 函数返回值

返回值由`returns(type)`的方式声明，且可以通过返回元祖的方式声明多个返回值。

## view函数

标志为view函数，表明函数不会修改合约状态。



## pure函数

标识为pure的函数，表明函数不但不会修改合约状态，也不会读取合约状态。



## 函数修饰符

可以通过声明的方式，对函数本体的代码进行包裹和定制。

有点类似注解。

函数修饰符内的占位符`_`可以导致函数被多次执行。

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity >=0.7.1 <0.9.0;

contract FunctionModifierRunTwice {
  int256 public a;

  modifier runTwice() {
    // 连着使用两个函数代码占位符，导致函数代码被执行两次
    _;
    _;
  }

  // 由于被 runTwice 修饰，实际上将导致每调用一次，a 将被增加 2
  function addOne() public runTwice {
    a++;
  }
}
```

支持函数重载





## 可支付函数

当一个函数带有`payable`标志时，表示该函数可用于接受以太币。

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity >=0.7.1 <0.9.0;

contract FunctionPayable {
  function pay() public payable {
    require(msg.value == 1 ether, unicode"只能支付1个以太币");
  }
}
```



## receive函数



当使用send以及transfer方法发送以太币时，将被receive函数接受。

receive函数的签名必须是`receive() external payable {}`，注意前面不需要加`function`关键字。

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity >=0.7.1 <0.9.0;

contract FunctionReceive {
  receive() external payable {
    require(msg.value == 1 ether, unicode"只能支付1个以太币");
  }
}
```

一个合约只能有一个`receive`函数.

## fallback函数

fallback是一个兜底函数，当外部发起一个调用，但是外部要调用的函数签名与合约中的任何函数都不匹配时，将调用 `fallback` 函数进行兜底处理，例如拒绝该交易、发出错误事件等等。
