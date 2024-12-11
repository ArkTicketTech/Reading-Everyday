# 16 Collision Resistance 2: constructions

## The Merkle-Damgård iterated construction

给定压缩函数 $h: T \times X \longrightarrow T$ 

Merkle-Damgård 构造定义如下：

$H_0 = IV$

$H_i = h(H_{i-1}, M_i)$ 其中 $i = 1,\dots,\ell$

$H(M) = H_\ell$

其中 $PB$ 是填充函数，一定要包含消息长度信息，形如 $1 || 0^k || Msg Length$，$M_i$ 是消息的第 $i$ 个块。

## MD collision resistance

### 定理
如果 $h$ 具有碰撞抗性，那么 $H$ 也具有碰撞抗性。

### 证明
通过归约：碰撞在 $H$ 上 $\Rightarrow$ 碰撞在 $h$ 上

假设 $H(M) = H(M')$，我们来构造 $h$ 的碰撞。

- 设 $M$ 被填充并分块为 $M_1, M_2, ..., M_k$
- 设 $M'$ 被填充并分块为 $M'_1, M'_2, ..., M'_{k'}$
- 定义中间链式变量：
  - $H_i = h(H_{i-1}, M_i)$
  - $H'_i = h(H'_{i-1}, M'_i)$

如果在某个位置 $i$ 有：
- $H_{i-1} = H'_{i-1}$ 但 $M_i \neq M'_i$ 或者 $H_{i-1} \neq H'_{i-1}$
- 则找到了 $h$ 的碰撞，因为：
  $h(H_{i-1}, M_i) = h(H'_{i-1}, M'_i)$ 但输入不同

而 $H(M) = H(M')$， 因此要么 $H_{i-1} = H'_{i-1}$ 且 $M_{i-1} = M'_{i-1}$，要么已经找到了 $h$ 的碰撞。 对于前者可以一直往前递归，直到找到碰撞。如果找不到则说明 $M = M'$，这与假设矛盾。

因此，如果能在 $H$ 上找到碰撞，必然能在 $h$ 上找到碰撞。
这与 $h$ 的碰撞抗性假设矛盾，所以 $H$ 必须也是碰撞抗性的。
