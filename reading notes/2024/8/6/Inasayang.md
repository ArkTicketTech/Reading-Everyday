Solidity: 编写以太坊虚拟机（EVM）智能合约的编程语言

https://remix.ethereum.org

- 值类型
  - Boolean
  - Integer
    - int8 到 int256 / uint8 到 uint256: 8位到256位的整数
    - 以 8 位为步长递增
    - int24, int40等等也是可以，但不常用
  - Address
    - address 和 address payable 都是固定大小的 20 字节（160 位）
    - address: 基本地址类型，可以存储以太坊地址
    - address payable: 可支付地址类型，除了存储地址外，还可以接收以太币
  - Fixed Point Numbers
  - Enums
  - Byte Arrays
    - bytesN，其中 N 可以是 1 到 32 之间的任何整数
    - bytes 用于表示动态大小的字节数组
- 引用类型
  - Arrays
  - Structs
  - Mappings
    - mapping(KeyType => ValueType) visibility variableName
    - KeyType 可以是任何内置的值类型,不能是引用类型
    - ValueType: 可以是任何类型，包括另一个 mapping 或结构体
- Function
  - function (<parameter types>) {internal|external} [pure|view|payable] [returns (<return types>)]
  - pure 既不能读取也不能写入, 调用 pure 函数不需要支付 gas, 主要提供纯计算、提高代码质量
  - view 能读取但也不能写入
- Contract
- String
- 特殊
  - 时间单位
    - seconds, minutes, hours, days, weeks
  - 以太单位
    - wei (基本单位)，以太坊中最小的货币单位
    - gwei (10^9 wei)
    - ether (10^18 wei)
    - kwei (1000 wei)
    - mwei (10^6 wei)
    - szabo (10^12 wei)
    - finney (10^15 wei)

``` 
pragma solidity ^0.8.0;

contract DataTypesExample {
    // 值类型
    bool public isActive = true;
    int public signedNumber = -10;
    uint public unsignedNumber = 10;
    address public owner;
    bytes32 public data = "Hello, Solidity!";

    // 枚举
    enum Status { Pending, Approved, Rejected }
    Status public currentStatus = Status.Pending;

    // 数组
    uint[5] public fixedArray = [1, 2, 3, 4, 5];
    uint[] public dynamicArray;

    // 结构体
    struct Person {
        string name;
        uint age;
        address wallet;
    }
    Person public person;

    // 映射
    mapping(address => uint) public balances;

    // 字符串
    string public greeting = "Hello, World!";

    constructor() {
        owner = msg.sender;
    }

    // 函数类型
    function operation(uint x, uint y, function(uint, uint) pure returns (uint) func) public pure returns (uint) {
        return func(x, y);
    }

    function add(uint x, uint y) public pure returns (uint) {
        return x + y;
    }

    // 使用时间单位
    uint public oneDay = 1 days;

    // 使用以太单位
    uint public oneEther = 1 ether;

    function setPersonInfo(string memory _name, uint _age) public {
        person = Person(_name, _age, msg.sender);
    }

    function addToArray(uint _number) public {
        dynamicArray.push(_number);
    }

    function setBalance(address _account, uint _balance) public {
        balances[_account] = _balance;
    }

    function useOperation() public view returns (uint) {
        return operation(10, 20, add);
    }
}
```

gas fee：
  - 类比高速公路的通行费
  - 使用付费：只有使用高速公路（进行交易）时才需要付费
  - 基于使用量：费用取决于行驶距离（操作复杂度）
  - 拥堵定价：在交通高峰期（网络拥堵时）费用更高
  - 选择性：可以选择是否使用高速公路（是否进行交易）
  - 维护网络：费用用于维护和改善道路（区块链网络）