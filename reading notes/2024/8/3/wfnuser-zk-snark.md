已知 t(x) 需要证明 p(x)

验证者生成随机 r - 如果直接将 r 公布给证明者
证明者甚至直接返回任意 p = h * t(r) 即可

为了规避这一点

验证者要将 r 放到黑箱里

E(r) = g^r % n

改成公布 E(r) 还有 E(r) E(r^2) ...

-----

prover 先计算 h(x)

p(x) = h(x) * t(x)

E(p(x)) = E(h(x))^t(x)

E(a*x^3+b*x^2) = E(x^3)^a * E(x^2)^b * ...

prover 根据 E(r) E(r^2) ... 
计算出 E(p(r)) 和 E(h(r))

---- 

verifier 验证 E(p(r)) = E(h(r)) ^ t(r)

此时 prover 不知道 r 是什么 - 因此不能随意构造 h(r) 和 p(r) 返回


