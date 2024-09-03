# Build A Decentralized Monetary System
if central bank plans to issue digital currency, it's intuitive to refer to the characteristic of cash.

1. **Counterfeit Resistance**: Difficult to forge, unalterable.
2. **Uniqueness**: Non-reproducible.

Counterfeit resistance can be effectively addressed through a **public-private key system**.

Uniqueness is crucial because code, as a file, can be infinitely copied, and the inability to ensure uniqueness can lead to **double-spending attacks**, where a single currency is spent multiple times.

Maintaining the uniqueness of currency, the solution for **centralized monetary systems** is centralized accounting, which preserves the relationship between currency and holders.

However, this approach has the drawback of requiring confirmation from the central bank system for each transaction.

The solution for **decentralized monetary systems** is blockchain technology.


```plain
               ┌───────────┐    ┌──────────────┐     ┌─────────────┐              
               │           │    │              │     │             │              
 ┌─────────────┼────┐     ┌┼────▼──────┐     ┌─┼─────▼──────┐    ┌─┼────────────┐ 
 │ create coin │    │     │A┌──►B(5)   │     │ B┌───►C(2)   │    │ C────►E(7)   │ 
 │             ▼    │     │ │          │     │  │           │    │ │            │ 
 │       ─────►A(10)│◄────┤ │          │◄────┤  │           │◄───┤ │            │ 
 │                  │     │ └──►C(5)   │     │  └───►D(3)   │    │ │            │ 
 │                  │     │ signed by A│     │  signed by B │    │ │signed by C │ 
 └──────────────────┘     └─────▲──────┘     └──────────────┘    └─┼────────────┘ 
                                │                                  │              
                                │                                  │              
                                └──────────────────────────────────┘              
                                                                                            
                                                                                  
```
Every Bitcoin transaction includes **inputs**, **outputs**, and **signature**.

1. Input section:
    1. Source of the coins: It traces the origin transaction of the coins to confirm the transaction's source, preventing the same currency from being spent multiple times.
    2. Sender's public key hash: Being able to cross-verify with the recipient's public key hash from the output part of the transaction where the coin originates ensures that the public key has not been forged.
2. Output section: Output section includes the recipient's public key hash;
3. Signature: The sender uses his private key to sign the transaction, which is then cross-verified with the sender's public key hash.


In the Bitcoin system, due to the potential presence of malicious nodes, it is necessary to determine which node has the right to write transactions into the blockchain. 
Bitcoin employs a voting mechanism based on computational power rather than account numbers to prevent **Sybil attacks**. 

This computational power-based voting, also known as **Proof of Work**, involves brute-forcing a system-preset random number `nonce`, with the node that solves it first gaining the right to write into the block and receiving BTC rewards.

Additionally, to prevent forking attacks, it is stipulated that only blocks on **the longest valid chain** are accepted, and blocks on non-longest valid chains are discarded.

```plain
                                                  ┌───────┐◄──┬───────┐ 
                                              ┌───┤       │   │       │ 
 ┌──────┐◄──┬──────┐ ◄───┬──────┐◄───┬──────┐ │   └───────┘   └───────┘ 
 │C->A  │   │      │ ◄┐  │ A->B │    │      │◄┘                         
 └──────┘   └──────┘  │  └──────┘    └──────┘◄┐   ┌───────┐             
                      │                       └───┼       │             
                      │                           └───────┘             
                      └──┬──────┐                 orphan block          
                         │A->A' │                                       
                         └──────┘                                       
```
