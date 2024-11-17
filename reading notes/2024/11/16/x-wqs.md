目标：安全的PRG => 流密码是语义安全的。

G:K->{0,1}^n 安全的PRG，如果G是安全的，那么从G提取的密钥流是语义安全的。无法证明完美保密性，因为流密码较短，不可能完全安全。

如何证明？
任意的语义安全攻击者A，存在一个PRG攻击者B，使得Adv[A,E] <= 2*Adv[B,G]。A对流密码有可忽略优势，B对PRG有可忽略优势。
![image](https://github.com/user-attachments/assets/fd6fe5e3-96e5-4ea0-a24c-f6a1a39d1fe7)
Challenger随机选一个秘钥，将GK加密切换为用r加密，用真正的随机填充而非伪随机填充。
攻击者无法察觉，因为生成器是安全的，无法赢得游戏，流密码一定是安全的。


proof let A be a semantic security adversary attacking E.
claim1: $$|Pr[R_{0}] - Pr[R_{1}]| = Adv_{ss}[A, OTP] = 0$$
claim2: 存在一个PRG攻击者B， $$|Pr[W_{b}] - Pr[R_{b}]| = Adv_{ss}[B, G]$$
=> $$Adv_{ss}[A, E] = |Pr[W_{0}] - Pr[W_{1}]| <= 2*Adv_{ss}[B, G] = 2*Adv_{prg}[B, G]$$

