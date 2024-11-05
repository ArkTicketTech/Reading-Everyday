
## Attack1: two time pad is insecure!!

Never use stream cipher key more than once !!

$C_1 =  m1 \oplus PRG(k)$

$C_2 =  m2 \oplus PRG(k)$

Eavesdropper does:

$C_1 \oplus C_2 = m_1 \oplus m_2$

Enough redundancy English and ASCII encoding that:

$m_1 \oplus m_2$ => you can recover m1, m2

So Never use stream cipher key more than once !!
