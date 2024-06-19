hnsw 的搜索路径长度是有上限的

$S = 1 / (1 - exp(-m_L))$

$O(M*d) -> O(M*d*log(\gamma * M))$

seach path 长度要乘 $O(\gamma)$
