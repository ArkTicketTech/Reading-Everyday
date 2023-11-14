- what  
	- We present NN-Descent, a simple yet efficient algorithm for approximate K-NNG construction with arbitrary similarity measures  
-  
-  
- formalized  
	- $N = \lVert V \rVert$   
	  $\sigma : V \times V \rightarrow \mathbb{R}$ 相似度衡量 令之为 $-d$  
	  $B_K(v)$ 表示 v 的 K-NN B[v]  
	  $R_K(v) = \{ u \in V \mid v \in B_K(u) \}$ 表示 reverse K-NN R[v]  
	  $\overline{B}[v] = B[v] \cup R[v]$  
	  $B_r(v)=\{ u \in V \mid d(u, v) \leq r \}$  
	  growth restricted if there exists a constant c, s.t. $\lVert B_{2r}(v) \rVert \leq c \lVert B_r(v) \rVert, \quad  \forall v \in V$  
-  
- Basic Algorithm  
	- for every v ∈ V , let B′[v] = S v′ ∈B[v] B[v′] be the set of points we explore trying to improve B  
		- $B'[v] = \cup_{v' \in B[v]}B[v']$  
	- that for certain fixed radius r, for all v ∈ V , B[v] contains K neighbors that are uniformly distributed in Br (v), then assuming independence of certain events and that k ≪ |Br/2(v)|, we can conclude that B′[v] is likely to contain K neighbors in Br/2(v)  
