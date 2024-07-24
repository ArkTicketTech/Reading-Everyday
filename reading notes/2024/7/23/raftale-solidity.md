demo：
一个简单的钱包

1. 可以向这个地址发送eth：
2. 可以withdraw

```sol
// SPDX-License-Identifier: MIT
pragma solidity 0.8.24;

contract EtherWallet {
    // 当一个函数带有payable标志时，表示该函数可用于接受以太币。
    address payable public ownerAdress;

    constructor() {
        // msg.sender是一个全局变量，指的是调用者的地址
        ownerAdress = payable(msg.sender);
    }

    // 当使用send以及transfer方法发送以太币时，将被receive函数接受
    receive() external payable { }

    function withdraw(uint _amount) external {
        require(msg.sender == ownerAdress, "caller is not owner");
        // transfer是一个函数，它可以用来在合约中向其他地址转移以太币，这个函数由ERC20标准定义，可以通过调用address.transfer()来使用
        ownerAdress.transfer(_amount);
    }

    function getBalance() external  view returns (uint) {
        // address(this) 引用当前合约地址
        return address(this).balance;
    }

}
```

在Remix中，合约已部署的情况下，如果想转账以太币，可以输入以太币数量（设置value），然后点击low level interactions 中的Transact进行转账。

合约中必须要有`receive()`。
