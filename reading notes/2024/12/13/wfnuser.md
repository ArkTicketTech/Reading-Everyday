## Degree Bound 证明

KZG10 支持证明多项式 $f(X)$ 的 Degree 小于等于 $d$。Prover 构造新的多项式：

$$
\tilde{f}(X) = X^{D-d} \cdot f(X)
$$

$\tilde{f}(X)$ 的 Degree 小于等于 $D$，承诺为：

$$
\pi_{\text{deg}} = [\tau^{D-d} \cdot f(\tau)]_1
$$

Verifier 验证方式：

$$
e\left([\tau^{D-d} \cdot f(\tau)]_1, [1]_2 \right) = e\left([f(\tau)]_1, [\tau^{D-d}]_2 \right)
$$

SRS 需要包括：

$$
SRS = \left(
[\tau^1]_1, [\tau^2]_1, \dots, [\tau^D]_1,
[\tau^1]_2, [\tau^2]_2, \dots, [\tau^{D-d}]_2
\right)
$$

若同时证明 Degree Bound 和 Evaluation，则 Prover 需产生 $([f(\tau)]_1, [\tau^{D-d} f(\tau)]_1)$，Verifier 需完成 4 个 Pairing 验证。