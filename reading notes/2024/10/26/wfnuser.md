- What is cryptography
	- Crypto magic
		- Privately outsourcing computation
		- Zero knowledge
	- A rigorous science
		- precisely specify threat model
		- propose a construction
		- prove that breaking construction under threat mode will solve and underlying hard problem
- History of Cryptography
	- symmetric ciphers
		-
		  ```
		        Alice (K)                               Bob (K)
		         ┌───┐                                 ┌───┐
		         │ E │                                 │ D │
		         └─┬─┘                                 └─┬─┘
		           │                                     │
		        m ─┘                                     │
		           │  Encryption: c = E(K, m)            │
		           │ ──────────────────────────────────> │
		           │             Internet                │
		           │                                     │
		           │       c  (cipher text)              │
		           │ ──────────────────────────────────> │
		           │  Decryption: m = D(K, c)            │
		           │                                     └──> m
		  
		  Legend:
		  - "K": Shared key known to both Alice and Bob.
		  - "E": Encryption function, takes message "m" and key "K" to produce ciphertext "c".
		  - "D": Decryption function, takes ciphertext "c" and key "K" to retrieve the original message "m".
		  ```