Suppose F:KxX -> y is a secure PRF with Y ={0,1}^10. Is the derived MAC I a secure MAC system?
Answer: The tag is too short, its not secure. The attacker choose a message m, and just guess the value of the MAC for the particular message. Because tag is only ten bits long. The attacker has a chance of one in two to the ten in guessing the MAC correctly. The adv is 1/1024, it's non negligible. The attacker will probabily forge (伪造) the MAC on a given message with the probability one in 1000 which is insecure.  

Only when the output of the function is small can things go wrong. If the output of the PRF is large, then we get a secure MAC out this function.Suppose we have a function F that takes messsages in X and outputs tags in Y. Then the MAC that's derived(导出) from this PRF, in fact is a secure MAC.

We want the output space be large, so the advantage can be negligible. And so taking a PRF that outputs 80 bits is perfectly fine. That will generate an 80-bit MAC. Therefore, the advantage of any adversary will be at most one over two to the 80. 
