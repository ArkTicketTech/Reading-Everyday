# 构建安全的MAC

## MAC的基本概念回顾
- MAC是一对算法:
  - 签名算法:接收消息和密钥作为输入，生成标签
  - 验证算法:接收密钥、消息和标签作为输入，输出0或1
- 安全性要求:在选择消息攻击下存在性不可伪造
  - 攻击者可以进行选择消息攻击
  - 无法创建有效的新消息-标签对

## 基于PRF构建MAC

### 基本构造方法
1. 使用PRF(伪随机函数)作为基础
2. 签名过程:对消息M计算PRF(M)作为标签
3. 验证过程:重新计算PRF(M)并与标签比对

### 不安全的例子
- 使用10位输出的PRF，构造MAC
- 问题:标签太短
- 攻击者有1/2^10的概率猜对MAC
- 1/1024的成功率是不可忽略的

### 安全性定理
- 如果PRF输出足够长，可以构建安全的MAC
- 安全要求:
  - PRF必须是安全的
  - 输出空间要足够大(如80位)
  - 攻击者优势最多为1/2^80

## 安全性证明分析

### 基于真随机函数的分析
1. 考虑使用真随机函数而非PRF
2. 攻击者获取多个消息的标签
3. 对新消息M预测标签的成功率仅为1/Y (Y是输出空间大小)
4. 因为函数在新点的值与已知点完全独立，攻击者只能猜测标签值

### PRF的等效性
- 攻击者无法区分PRF的行为与真随机函数
- 攻击者在PRF上的优势同样被限制在1/Y

## 实际应用扩展

### 从小MAC到大MAC
- AES作为基础PRF(16字节，128位输入)
- 两种构造方法:
  1. CBC-MAC:银行业常用(如自动清算所ACH系统，确保支票在银行间传输完整性)
  2. HMAC:互联网协议常用(SSL/IPsec/SSH)
- 从接收小消息的PRF开始，产生可处理GB级长消息的PRF，都可以用AES作为底层密码来实现。

### MAC输出截断
- N位PRF可安全截断至T位(T<N)
- 截断后仍保持安全性
- 实践建议:
  - 不应截断至过短(如3位)
  - 80位或64位是合理的选择
  - 无需保持完整的128位输出
