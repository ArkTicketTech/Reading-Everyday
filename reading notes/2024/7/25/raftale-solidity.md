ERC20接口：

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

interface IERC20 {
  // Returns the total token supply
  function totalSupply() external view returns (uint256);

  // 返回账户owner中的余额
  function balanceOf(address account) external view returns (uint256);

  // 向recipient转账amount
  function transfer(address recipient, uint256 amount)
  external
  returns (bool);

  // 授权的账户还有多少授权的钱可以用
  function allowance(address owner, address spender)
  external
  view
  returns (uint256);

  // 授权spender从你的账户中多次"提取"token,直到规定的金额amount
  function approve(address spender, uint256 amount) external returns (bool);


  // move `amount` tokens from `sender` to `recipient` using the allowance mechanism
  function transferFrom(address sender, address recipient, uint256 amount)
  external
  returns (bool);
}
```



1. transfer(recipient, amount): 当前账户调用transfer向recipient转账amount个token。
2. approve(sender, amount)：授权spender可以从该账户转移amount个token
3. transferFrom(sender, recipient, amount)：用于sender从当前账户转移到目标账户recipient。
4. allowance(owner, spender)：返回spender还能从owner账户提取的token个数。



比如：假设当前账户A有100个token，

1. approve(B, 10)：授权B可以调用其中10个代币
2. transferFrom(A, C, 10)：B能从A转10个代币给C
3. allowance(A, B)：查看B还可以调用A账户token的剩余个数



```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import "./IERC20.sol";

contract ERC20 is IERC20 {

  event Transfer(address indexed from, address indexed to, uint256 value);

  event Approval(
    address indexed owner, address indexed spender, uint256 value
  );

  uint256 public totalSupply;
  // 每个地址的余额
  mapping(address => uint256) public balanceOf;
  // 授权的账户还有多少余额可以使用
  // allowance[x][y]：y可以从x中调用的货币余额
  mapping(address => mapping(address => uint256)) public allowance;
  // 代币名称
  string public name;
  // 简称
  string public symbol;
  // 小数点，一般是18
  uint8 public decimals;
  // 构造函数
  constructor(string memory _name, string memory _symbol, uint8 _decimals) {
    name = _name;
    symbol = _symbol;
    decimals = _decimals;
  }

  function transfer(address recipient, uint256 amount)
  external
  returns (bool)
  {
    balanceOf[msg.sender] -= amount;
    balanceOf[recipient] += amount;
    emit Transfer(msg.sender, recipient, amount);
    return true;
  }
  // 授权sender可以调用amount个代币
  function approve(address spender, uint256 amount) external returns (bool) {
    // spender可以从当前账户调用amount个代币
    allowance[msg.sender][spender] = amount;
    emit Approval(msg.sender, spender, amount);
    return true;
  }
  // 第三方能从sender账户转给recipient amount个代币
  function transferFrom(address sender, address recipient, uint256 amount)
  external
  returns (bool)
  {
    // 当前调用者可以从sender账户调用的货币余额
    allowance[sender][msg.sender] -= amount;
    balanceOf[sender] -= amount;
    balanceOf[recipient] += amount;
    emit Transfer(sender, recipient, amount);
    return true;
  }

  // 创造token
  function _mint(address to, uint256 amount) internal {
    balanceOf[to] += amount;
    totalSupply += amount;
    emit Transfer(address(0), to, amount);
  }
  // 销毁token
  function _burn(address from, uint256 amount) internal {
    balanceOf[from] -= amount;
    totalSupply -= amount;
    emit Transfer(from, address(0), amount);
  }
  // 创造token
  function mint(address to, uint256 amount) external {
    _mint(to, amount);
  }
  // 销毁token
  function burn(address from, uint256 amount) external {
    _burn(from, amount);
  }
}
```

## Create your own ERC20 token

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import "./ERC20.sol";

contract MyToken is ERC20 {
  constructor(string memory name, string memory symbol, uint8 decimals)
  ERC20(name, symbol, decimals)
  {
    // Mint 100 tokens to msg.sender
    // Similar to how
    // 1 dollar = 100 cents
    // 1 token = 1 * (10 ** decimals)
    _mint(msg.sender, 100 * 10 ** uint256(decimals));
  }
}
```

## Contract to swap tokens

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import "./IERC20.sol";

/*
How to swap tokens

1. Alice has 100 tokens from AliceCoin, which is a ERC20 token.
2. Bob has 100 tokens from BobCoin, which is also a ERC20 token.
3. Alice and Bob wants to trade 10 AliceCoin for 20 BobCoin.
4. Alice or Bob deploys TokenSwap
5. Alice approves TokenSwap to withdraw 10 tokens from AliceCoin
6. Bob approves TokenSwap to withdraw 20 tokens from BobCoin
7. Alice or Bob calls TokenSwap.swap()
8. Alice and Bob traded tokens successfully.
*/

contract TokenSwap {
  IERC20 public token1;
  address public owner1;
  uint256 public amount1;

  IERC20 public token2;
  address public owner2;
  uint256 public amount2;

  constructor(
    address _token1,
    address _owner1,
    uint256 _amount1,
    address _token2,
    address _owner2,
    uint256 _amount2
  ) {
    token1 = IERC20(_token1);
    owner1 = _owner1;
    amount1 = _amount1;
    token2 = IERC20(_token2);
    owner2 = _owner2;
    amount2 = _amount2;
  }


  function swap() public {
    require(msg.sender == owner1 || msg.sender == owner2, "Not authorized");
    require(
      token1.allowance(owner1, address(this)) >= amount1,
      "Token 1 allowance too low"
    );
    require(
      token2.allowance(owner2, address(this)) >= amount2,
      "Token 2 allowance too low"
    );

    _safeTransferFrom(token1, owner1, owner2, amount1);
    _safeTransferFrom(token2, owner2, owner1, amount2);
  }

  function _safeTransferFrom(
    IERC20 token,
    address sender,
    address recipient,
    uint256 amount
  ) private {
    bool sent = token.transferFrom(sender, recipient, amount);
    require(sent, "Token transfer failed");
  }
}
```



solidity的代码不难看懂，难点应该是对区块链和业务的理解。

