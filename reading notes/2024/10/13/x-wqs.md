**Q** If one more uncle block appears while mining the next block, how can it be included?

**A** By modifying the fields in the block header to include two uncle blocks. This modification has no loss for mining because the mining process is unmemorable. Modifying and continuing to mine is the same as setting the parameters from the beginning, and there is no disadvantage.

**Q** Why can only two blocks be included?

**A** If it is considered an uncle block, it can obtain 7/8 of the block reward, which is very high. If there is no limit and 100 uncles can be recognized, then Ether will be too worthless.


**Q** When an uncle block is included, should the transactions in the uncle block be executed?

**A** They should not be executed. Ethereum and Bitcoin are transaction-driven state machines. Every time a new block is published on the chain, the current state will be transferred to the next state. There may be conflicts between transactions on the main chain and uncle blocks. Ethereum prevents double-spending attacks through account balances. Two different transactions may conflict. If the transactions of uncle blocks are executed, some transactions may become illegal transactions. The uncle block itself is not necessarily illegal. After executing the parent block and then executing the uncle block, it may become illegal.

**Q** Is it possible to check which transactions in the uncle block are legal and only execute legal transactions?

**A** To ensure transactions along the main chain, the transactions of uncle blocks are not executed by the current block, and the legality of uncle block transactions is not checked. The current block will check whether the uncle block is legally published and whether it meets the mining difficulty by checking the block header.

**Q** How can we detect how many uncle blocks there are?

**A**  Check when one is monitored. Each published block must indicate the parent block, so it is possible to detect whether there is a common ancestor between the uncle block and the current block.

**Q** If there is a series of blocks following after a fork, should the entire chain be regarded as uncle blocks and rewarded to encourage merging?

**A** Only the first block has a reward, and the rest have no rewards. The cost of forking attack is too low. If the entire chain is rewarded and the forking attack is successful and the transaction is rolled back, otherwise, block rewards can still be obtained. Then the cost of forking attack will be greatly reduced.

```plain
ETH
          |←------at most 7 generation------→|
  ←  □ ←  □ ←  □ ←  □ ←  □ ←  □ ←  □ ←  □ ←  □  
            ↖     
               □ ←  □ ←  □ ←  □ ←  □ ←  □ ←  □
            reward  0    0    0    0    0    0
```
