**MPC-in-Multi-Heads: A Multi-Prover Zero-Knowledge Proof System**

**How to Jointly Prove Any NP Statements in ZK**

**Abstract**

zero-knowledge proofs(ZKP): less adequate for privacy-preserving applications in the distributed setting.

传统的ZKP零知识证明系统在分布式环境的隐私保护应用中是远远不够的。

“double financing”双重融资 ”credit checking”信贷检查 multi-prover applications

“range proof” 

formalizing the ZKP system in the multi-prover setting(MPZK)

under the simulation-based paradigm

**1 Introduction**

AKP is a powerful cryptographic primitive that enables a prover to convince a verifier of the membership of a problem instance in an NP language without revealing anything substantial beyond its validity.

non-interactive zero-knowledge(NIZK)

the computational complexity of proving and verification and the proof size(communication complexity in general)

**computational soundness** (commonly referred to as **argument systems**)

build succinct argument systems with **proof size and verification complexity** sub-linear in the size of the **statement**.



Double Financing:

the completeness theorems of multiparty computation(MPC) 

general MPC frameworks: arithmetic or Boolean circuits 算术或布尔电路



ZKP in the MPC-in-the-Head paradigm: Prover first simulates the multiparty verification process of some NP relation locally and then commits to the generated views, a random subset of which will be challenged for consistency checking.

technical hurdles:

1)MPC-in-the-Head到MPC-in-multi-Heads: inter-prover communication

2)inconsistency between inter-prover interaction and that implicit in the proof string



in the multi-prover setting=distributed manner



classical Fiat-Shamir transform compiles a constant-road public-coin protocol into a non-interactive one in the random oracle model.

**1.1 Related Works**

PV-MPC ID-MPC

A weaker notion of “auditabilty” is introduced where the certificate only shows cheating occurs but does not pinpoint the cheater.

our construction could preserve zero-knowledge even if all-but-one prover is corrupted. 只要一个证明者是诚实的(而其他人可以任意勾结)



The witness in our case is shared across multiple provers, and the proof system offers protection of the privacy of witness shares.



1 the witness can be shared arbitrarily conditioned on the existence of efficient reconstruction

2 threshold CNF sharing: our construction offers efficient communication with a large number of provers.

3 key component: fully-linear PCP generalizing the linear PCP (causing the reliance on central prover)



multi-prover ZKPoK: actively-secure MPC protocols

**our constructions can be applied to any NP languages.**



Multi-prover Interactive Proofs. MIP

our notion focuses on **NP relations**, and multiple provers do communicate with each other since individual prover is presumed unable to convince the verifier.



**2 Preliminaries**

the symbols and notions of this work

build our proof system

多方计算multiparty computation

**2.1 Basic Notations**

negl(n)

poly(n)

PPT: probabilistic polynomial-time 概率多项式时间 

**2.2 Secure Computation**

**2.3 Helper Functionalities**



**4 MPC-in-Multi-Heads: A Black-Box Construction from MPC**

**4.1 Intuitions**

no single prover can complete the view generation process.



Choosing Inner Protocol: 

protected against the verifier

against each other(partial witness privacy)



Partial witness privacy（部分见证隐私）指的是在多方计算（MPC）中，参与者只能从整体上推断出一个参与者的私有信息，而不能推断出这个参与者的具体私有信息。这个概念主要是应用在零知识证明中，保证证明的安全性和隐私性。

举个例子，如果有三个参与者在进行MPC，每个参与者拥有一个私有输入，那么部分见证隐私要求除了这个参与者本身，其他参与者无法推断出这个参与者的具体输入，只能知道这个输入的某些属性或约束条件。

部分见证隐私是MPC中的重要概念，可以保证参与者的私有信息不被泄露，同时不影响计算的正确性。



**6 Conclusion and Future Directions**

formalize the syntax and security requirements for multi-prover zero-knowledge, the multi-prover extension of ZKP for proving arbitrary NP statements with distributed witnesses.



这篇论文提出了一种新的多证明者零知识证明系统，称为MPC-in-Multi-Heads（多头MPC）。多头MPC系统基于密码学中的安全多方计算（MPC）协议，它允许多个参与者协作完成计算而不会泄露其输入。该系统使用了多个证明者，每个证明者都负责验证不同的计算结果。这种方法可以使验证更加高效和灵活，并减少对单个证明者的依赖。

多头MPC系统通过使用阈值签名和加密技术来确保证明者之间的安全和保密性。每个证明者都持有其私钥，同时还需要多个证明者共同签名以发布证明。这使得单个证明者无法通过篡改证明来欺骗系统。

该系统还包括一种新的证明构造方法，称为“Chameleon Commitment”。该方法允许证明者选择他们的证明内容，从而使证明者可以自由地选择证明内容，并在不泄露其私密输入的情况下生成证明。

该论文的作者在实验中证明了多头MPC系统的有效性和实用性。他们在多个具有挑战性的问题上测试了该系统，并发现它能够在大多数情况下快速生成高效的证明。



多方安全计算（Secure Multi-Party Computation） 应用:

基因诊断

机器学习训练模型

联合风控问题



提倡一种新的ZK度量，称为"MPC友好性"，以捕捉多方设置中高效可计算的特性，从而更适合于多证明者应用。