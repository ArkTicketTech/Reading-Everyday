重读《算法竞赛入门经典》

并查集（Union-Find Set）的精妙之处在于用树来表示集合。

树的形态无关紧要，每棵树的根节点是这棵树所对应的集合的代表元（representative）。

并查集中的路径压缩方式就是来自于这句话「树的形态无关紧要」，并不需要在查找操作之后保持树的形态不变，只要顺便把遍历过的节点都改成树根的儿子。

```go
func find(x int) int {
    if p[x] == x {
        return x
    } else {
        p[x] = find(p[x])
    }
}
```

并查集效率非常高，在平摊意义上 find 函数的时间复杂度可以看成是常数。

平摊分析不看某一次花费时间，而是多次操作的平均时间。
