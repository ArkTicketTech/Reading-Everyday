公钥加密系统，一个是基于trapdoor函数，另一个是基于RSA。另一个是基于DiffeHellman函数。其实都遵循一个通用原则，即单向函数。在任何点计算函数f都是可以的，但是翻转f是困难的。

离散对数单向函数  DLOG one way function
确定一个阶为n的循环群G，如果查询g的所有幂，基本涵盖了整个大写G群。如果大写G中的离散对数问题很难，f就是一个单向函数。

正是这个单向函数的附加性质，使得公钥密码学成为可能。

RSA单向函数具有一个trapdoor。
