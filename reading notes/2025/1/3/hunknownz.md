Storage is designed as a word-addressed word array. Unlike memory, storage is associated with an Ethereum account and is persisted across transactions as part of the world state.

We've been running a contract account's bytecode all this time. Only now we see the account and the world state, and it matches the code inside the EVM.

Again, it’s important to note that storage is not part of the EVM itself, rather the currently executing contract account.
