remain to be done
- 数字签名，如何任何人都能验证的方式认证数据的真实性
- 认证密钥交换
- 用户认证，如何管理密码，一次性密码，挑战响应协议
- 隐私机制，如何在不透露你在哪里的情况下，进行自我认证，不透露你是谁的方式登录
- 零知识证明，在密码学中广泛使用的通用工具

final words
- 密码学是一个巨大的工具，在使用时应该始终小心，如果不正确的实施密码学，系统将完美的正常工作，没有办法知道有什么问题，除非当攻击者试图攻击系统，容易被攻破。
- 仅有密码学的一点点知识，非常危险。确保正确的实施非常重要。确保总是有其他人在审查你的代码和设计，以在密码学中找到任何漏洞，或者甚至在系统设计中，更普遍的漏洞。
- 永远不要自己发明密码或模式，并且永远甚至不要自己实现密码或模式。尽量尽可能遵循标准，遵循算法的标准实现。如果必须要偏离，确保对你所做的有足够的第三方审查。