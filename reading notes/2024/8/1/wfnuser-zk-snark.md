Non-Interactive Zero-Knowledge of a Polynomial

d-阶多项式蕴含 d 个不同的数字

• Verifier samples a random value r, calculates t = t(r) (i.e., evaluates) and gives r to the prover
• Prover calculates h(x) = p(x) and evaluates p(r) and h(r); the resulting values p,h are t(x)
provided to the verifier
• Verifier then checks that p = t · h, if so those polynomials are equal, meaning that p(x)
has t(x) as a cofactor.

但是这样是很容易作弊的

如果这些传递的信息本身是在黑箱里，但是又可以被操作🤔 就可以解决这个问题

同态加密就可以解决

