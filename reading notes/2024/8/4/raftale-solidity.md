
背景：学习hackquest.io的solidity开发教程。

任务：完成创建一个Fungible Token的小demo，并在hackquest dev环境和evm的测试链上操作

Fungible 意为可互换


结果：

一个token的三大要素，可以想象成黄金：

1. _balances：分配给各个账户的token数量
2. _totalSupply: 总生产数量
3. _owner：发布这个token的地址，拥有生产token的权利。

创建的Fungible Token代码如下。

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.26;

contract MyToken {
  //mapping used to store the balance corresponding to each address
  mapping (address => uint256) private balances;
  //A uint256 variable is used to store the total supply of the token. It is defined as public and can be queried by anyone.
  uint256 public totalSupply;
  //An address variable is used to store the issuer of this token. This is used for some permission control.
  address private owner;

  constructor(uint256 initialSupply) {
    owner = msg.sender;
    mint(msg.sender, initialSupply);
  }
  //function used to mint tokens
  function mint(address recipient, uint256 amount) public {
    //Permission control is implemented
    require(msg.sender == owner,"Only the owner can perform this action");
    balances[recipient] += amount;
    totalSupply += amount;
}
  //A function used to query the balance corresponding to an address
  function balanceOf(address account) public view returns (uint256) {
    return balances[account];
  }
  //A function used to transfer
  function transfer(address recipient, uint256 amount) public returns (bool) {
    require(amount <= balances[msg.sender], "Not enough balance.");
    balances[msg.sender] -= amount;
    balances[recipient] += amount;
    return true;
  }
}
```

如何在测试链上操作：

1. metamask领取sepolia测试网络，从https://faucets.chain.link/sepolia领取eth（部署合约需要消耗gas）。
2. ide.hackquest.io连接metamask：手动添加，在metamask已连接的网站中点击「手动连接到当前站点」
3. ide.hackquest.io上的Environment选择Inject Provider - MetaMask（brave浏览器上没有显示metamask，用chrome就可以）
4. 部署合约，操作合约函数。用metamask上的账户地址进行mint和transfer操作即可。
