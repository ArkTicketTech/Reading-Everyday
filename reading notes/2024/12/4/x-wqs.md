Attacker's power：chosen message attack
for m1,m2,m3,...,mq  attacker is given ti <- S(k,mi) 

Attacker's goal: existential forgery
produce some new valid message/tag pair(m,t)
Secure MACS

Let I=(S,V) be a MAC
Suppose an attacker is able to find m0 != m1 such that S(k,m0)=S(k,m1) for 1/2 of the keys k in K
this MAC can be broken using a chosen msg attack.
