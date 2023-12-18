每条规则都写成若干个横线上的前提断言和横线下的唯一的结论断言。如果所有的前提都满足，则结论一定成立；类型规则可以没有前提。每条规则都有一个名字（按照惯例，名称的第一个单词由结论断言决定；例如，形式为“(Val ... )”的规则名称用于其结论为值类型断言的规则）。在需要时，限制规则适用性的条件，以及规则内部使用的缩写，都会注释在规则名称或前提旁边。

例如，以下两个规则中的第一个规则说明任何数字都是类型为 $Nat$ 的表达式，可以出现在任何良构的环境 $\Gamma$ 中。第二个规则说明两个表示自然数的表达式 $M$ 和 $N$ 可以组合成更大的表达式 $M+N$，该表达式也表示自然数。此外，表达式 $M$ 和 $N$ 的环境 $\Gamma$ ，声明了 $M$ 和 $N$ 的任何自由变量的类型，也适用于 $M+N$。

$$
\begin{aligned}
& (Val\ n) \quad (n = 0, 1, \ldots) \\
& \frac
  {\Gamma \vdash \lozenge}
  {\Gamma \vdash n: Nat}
\end{aligned}
$$

$$
\begin{aligned}
& (Val\ +) \\
& \frac
  {\Gamma \vdash M : Nat    \Gamma \vdash N : Nat }
  {\Gamma\vdash M+N: Nat}
\end{aligned}
$$