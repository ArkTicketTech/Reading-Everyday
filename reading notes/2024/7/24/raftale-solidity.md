多重签名的作用是提高交易的安全性和可信度，必须要多个私钥都进行确认后才能进行交易。

多签钱包必须要有的功能：

1. 创建交易合约：submitTransaction
2. 交易确认：
3. 撤回确认
4. 执行交易：


```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

/**
多重签名钱包的智能合约，它可以让多个所有者

*/
contract MultiSigWallet {
  // event 是一个用于记录合约活动的接口，它可以将事件名、参数等存储到交易的日志中，并且可以被外部的监听器捕获和处理。
  event Deposit(address indexed sender, uint256 amount, uint256 balance);
  event SubmitTransaction(
    address indexed owner,
    uint256 indexed txIndex,
    address indexed to,
    uint256 value,
    bytes data
  );
  event ConfirmTransaction(address indexed owner, uint256 indexed txIndex);
  event RevokeConfirmation(address indexed owner, uint256 indexed txIndex);
  event ExecuteTransaction(address indexed owner, uint256 indexed txIndex);

  // 所有者列表
  address[] public owners;
  // 所有者映射
  mapping(address => bool) public isOwner;
  // 执行交易所需所有者确认数
  uint256 public numConfirmationsRequired;

  // 交易结构
  struct Transaction {
    // 发送到哪个地址
    address to;
    // 发送的量
    uint256 value;
    // 附带消息
    bytes data;
    // 是否被执行
    bool executed;
    // 确认数
    uint256 numConfirmations;
  }

  // 交易确认映射
  mapping(uint256 => mapping(address => bool)) public isConfirmed;
  // 交易数组
  Transaction[] public transactions;
  // 函数修饰
  modifier onlyOwner() {
    require(isOwner[msg.sender], "not owner");
    _;
  }

  modifier txExists(uint256 _txIndex) {
    require(_txIndex < transactions.length, "tx does not exist");
    _;
  }

  modifier notExecuted(uint256 _txIndex) {
    require(!transactions[_txIndex].executed, "tx already executed");
    _;
  }

  modifier notConfirmed(uint256 _txIndex) {
    require(!isConfirmed[_txIndex][msg.sender], "tx already confirmed");
    _;
  }
  // 传入所有者列表 和所需确认数
  constructor(address[] memory _owners, uint256 _numConfirmationsRequired) {
    require(_owners.length > 0, "owners required");
    require(
      _numConfirmationsRequired > 0
      && _numConfirmationsRequired <= _owners.length,
      "invalid number of required confirmations"
    );

    for (uint256 i = 0; i < _owners.length; i++) {
      address owner = _owners[i];

      require(owner != address(0), "invalid owner");
      require(!isOwner[owner], "owner not unique");

      isOwner[owner] = true;
      owners.push(owner);
    }

    numConfirmationsRequired = _numConfirmationsRequired;
  }
  // 接受以太币，并记录到日志中
  receive() external payable {
    emit Deposit(msg.sender, msg.value, address(this).balance);
  }
  // 提交交易，
  function submitTransaction(address _to, uint256 _value, bytes memory _data)
  public
  onlyOwner
  {
    // 交易索引下标
    uint256 txIndex = transactions.length;

    transactions.push(
      Transaction({
        to: _to,
        value: _value,
        data: _data,
        executed: false,
        numConfirmations: 0
      })
    );

    emit SubmitTransaction(msg.sender, txIndex, _to, _value, _data);
  }
  // 只能对未执行、未确认的交易进行确认
  // _txIndex 交易索引下标
  function confirmTransaction(uint256 _txIndex)
  public
  onlyOwner
  txExists(_txIndex)
  notExecuted(_txIndex)
  notConfirmed(_txIndex) 
  {
    Transaction storage transaction = transactions[_txIndex];
    transaction.numConfirmations += 1;
    isConfirmed[_txIndex][msg.sender] = true;

    emit ConfirmTransaction(msg.sender, _txIndex);
  }
  // 执行具体的转账交易
  function executeTransaction(uint256 _txIndex)
  public
  onlyOwner
  txExists(_txIndex)
  notExecuted(_txIndex)
  {
    Transaction storage transaction = transactions[_txIndex];
    // 检查确认数是否达标
    require(
      transaction.numConfirmations >= numConfirmationsRequired,
      "cannot execute tx"
    );

    transaction.executed = true;
    // 转账，transaction.to目标地址，call方法调用合约。
    // {value: transaction.value} 表示要发送的以太坊金额
    // (transaction.data) 表示调用合约时传入的数据
    (bool success,) =
    transaction.to.call{value: transaction.value}(transaction.data);
    require(success, "tx failed");

    emit ExecuteTransaction(msg.sender, _txIndex);
  }

  function revokeConfirmation(uint256 _txIndex)
  public
  onlyOwner
  txExists(_txIndex)
  notExecuted(_txIndex)
  {
    Transaction storage transaction = transactions[_txIndex];

    require(isConfirmed[_txIndex][msg.sender], "tx not confirmed");

    transaction.numConfirmations -= 1;
    isConfirmed[_txIndex][msg.sender] = false;

    emit RevokeConfirmation(msg.sender, _txIndex);
  }

  function getOwners() public view returns (address[] memory) {
    return owners;
  }

  function getTransactionCount() public view returns (uint256) {
    return transactions.length;
  }

  function getTransaction(uint256 _txIndex)
  public
  view
  returns (
    address to,
    uint256 value,
    bytes memory data,
    bool executed,
    uint256 numConfirmations
  )
  {
    Transaction storage transaction = transactions[_txIndex];

    return (
      transaction.to,
      transaction.value,
      transaction.data,
      transaction.executed,
      transaction.numConfirmations
    );
  }
}
```
