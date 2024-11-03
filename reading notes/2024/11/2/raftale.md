

# Vigenère cipher

encrypt:

${\displaystyle C_{i}=E_{K}(M_{i})=(M_{i}+K_{i}){\bmod {2}}6}$

假设预先知道key的长度l，那么每隔l个字母它们的cipher是一样的，因此可以猜测出现频率最高的可能是E(毕竟我们预先知道最高的是E)，最后也就能猜测key。如果最后的message是有意义的，那就说明破解是没有问题的。
即使我们不知道key的长度，也可以通过不断的假设key的长度，通过穷举key的长度来找到有意义的message。
