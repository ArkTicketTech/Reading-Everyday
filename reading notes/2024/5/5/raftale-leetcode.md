## 基本介绍

图是一种由顶点和边组成的一种数据结构，顶点是在图中的任意一个结点，边是两个结点之间的连接。

这种复杂的数据结构可以用来抽象更复杂的现实，比如地图可以看作是以十字路口为结点、公路为边的图，社交网络是由人作为结点、人之间的关系（关系也是看可以枚举和量化）作为边的图。

图的典型应用

图的类型有很多，我们先介绍三种图。

## 无向图

无向图是指任意两个顶点之间的边是没有方向的，没有方向意味着每条边都可以在两个方向上穿过。

## 加权无向图

不同的边可能有不同的权值；

## 有向图

有向图：任意两个顶点之间的边是都是有方向的，有方向意味每条边只能在单个方向上穿过 。

树可以看中一种特殊的图：有向的无环图，

## 加权有向图

加权图：每条边都带有一个相关的权重；

## 特殊的图

最特殊、最常见的图应该就是树了，当一个图是无环图，但添加任意一条边都会产生一条环，满足这个条件的图就是树，大多数树都是有向的。



## 图的存储

不管是什么类型的图，最重要的无非是顶点（vertex）和边（edge），假设顶点有V条，边有E条，如果要实现图的表示，需要满足下列两个条件：



1. 它必须为可能在应用中碰到的各种类型的图预留出足够的空间；
2. 图的实例方法的实现一定要快；



### 邻接矩阵(adjacency matrix)

邻接矩阵 是一个V*V的二维矩阵，一般我们使用`boolean`数组`arr`。



1. 如果`arr[i][j]`的值为`true`，表示点 i与点 j之间连了一条从`i`指向`j`的边。
2. 如果`arr[i][j]`的值为`false`，表示点 i与点 j之间没有连边。



二维矩阵arr添加一条边的时间复杂度为O(1)，判断两个边是否相连的时间复杂度也为O(1)，但是它有一个很明显的缺陷，就是存下一个图需要O(n^2)的空间复杂度，当点的个数远远小于边时，很多空间都被浪费了。

### 邻接表数组(An array of adjacency lists)

邻接矩阵在存储稀疏图的时候比较浪费空间，优化的思路就是我们不考虑每一个点对的连边信息，只考虑有连边的点对。我们可以将每个顶点的所有相邻顶点都保存在该顶点对应的元素所指向的一张链表中。


添加一条边的时间复杂度为O(1)，

当顶点可以用整数表示时，尤其是从0开始的连续的整数，普遍用线性表来表示：

对于无权图：以List<List<Integer>> graph存图，grap.get(u).get(v）来获取边(u,v)的信息

也可以用：List<Integer>[] graph = new ArrayList[n];  Arrays.setAll(graph, k -> new ArrayList<>());

加权图：List<List<int[]>> graph，int[] v_weight =graph.get(u); v = v_weight[0]; weight = v_weight[1]

或者是List<List<Pair>> graph.



当顶点不能用非负整数来表示时，比如字符串和对象引用类型，可以用哈希表来存图，key为顶点，value仍然是列表，存储顶点key的领接顶点。



不过还有另一种方法，映射对象引用的顶点到非负整数，然后用int[][] weights = new int[][] 存储顶点i到顶点j之间到权重。思想是这个思想，可以根据具体情况进行微调。



存储图的空间复杂度为O(E)，E为边的数量。

## 图的遍历

图的遍历如同树一样，使用DFS和BFS即可。

#### [323. 无向图中连通分量的数目](https://leetcode.cn/problems/number-of-connected-components-in-an-undirected-graph/)

该题的核心是将edges表示的图转化为邻接表数组，然后再通过dfs或者bfs遍历邻接表做标记，就跟之前的岛屿数量问题一样。



dfs的解法和bfs的解法的时间和空间复杂度都一样：



1. 时间复杂度：O(|V| + |E|) ，完成一遍图的遍历。
2. 空间复杂度：O(|V| + |E|) ，存图空间。

```java
class Solution {
  public int countComponents(int n, int[][] edges) {
    // 构造一个邻接表矩阵
    List<List<Integer>> graph = new ArrayList<>();
    for(int i = 0; i < n; i++) {
      graph.add(new ArrayList<>());
    }
    for(int[] edge : edges) {
      int u = edge[0];
      int v = edge[1];
      // 无向图
      graph.get(u).add(v);
      graph.get(v).add(u);            
    }
    // 图的显示
    for(int i = 0; i < n; i++) {
      List<Integer> nextNodes = graph.get(i);
      System.out.println(i + ": " + nextNodes);
    }

    boolean[] visited = new boolean[n];
    int count = 0;
    for(int i = 0; i < n; i++) {
      if(!visited[i]) {
        // 每一次dfs都会做标记，能从外部访问几次dfs就是连通分量的数量
        count++;
        visited[i] = true;
        // dfs(graph, i, visited);
        bfs(graph, i, visited);
      }
    }

    return count;
  }
  // dfs - 每成功访问一个结点后就做标记
  private void dfs(List<List<Integer>> graph, int i, boolean[] visited) {
    List<Integer> nextNodes = graph.get(i);
    for(Integer next : nextNodes) {
      if(!visited[next]) {
        visited[next] = true; 
        dfs(graph, next, visited);
      }
    }
  }

  // bfs - 每成功访问一个结点后就做标记
  private void bfs(List<List<Integer>> graph, int v, boolean[] visited) {
    Deque<Integer> queue = new LinkedList<>();
    queue.offer(v);
    visited[v] = true;
    while(!queue.isEmpty()) {
      int size = queue.size();
      for(int i = 0; i < size; i++) {
        Integer cur = queue.poll();
        List<Integer> nextNodes = graph.get(cur);
        for(Integer next : nextNodes) {
          if(!visited[next]) {
            queue.offer(next);
            visited[next] = true;
          }
        }
      }

    }
  }
}
```

#### [684. 冗余连接](https://leetcode.cn/problems/redundant-connection/)

冗余链接有个点需要注意：需要返回的是`edges` 中最后出现的边。



思路有三种：



思路一：从某个顶点v出发，执行bfs/dfs遍历，如果能找到v，就说明v是在环中；确定环上所有的顶点，然后对照edges，找出最后出现的边；



思路二：加边法 - 建图过程中，每加入一条边（u, v）前，搜索当前图上，能否从u搜索到v，如果可以，说明当前u、v已经连通，再加入这条边必定成环。由于我们从前到后依此取edges的边加入，最后一条成环的边就是答案；



思路三：减边法 - 先建好图，然后依此从edges的最后开始减掉这个边(u, v)，减边后搜索图中u、v是否连通，如果能连通说明(u, v)就是造成环的最后一条边。


dfs的加边法：

```java
class Solution {
  // 加边法
  public int[] findRedundantConnection(int[][] edges) {
    int n = edges.length;
    List<List<Integer>> graph = new ArrayList<>();
    // boolean[] visited = new boolean[n];
    for(int i = 0; i <= n; i++) {
      graph.add(new ArrayList<>());
    }
    for(int i = 0; i < n; i++) {
      int[] edge = edges[i];
      int u = edge[0];
      int v = edge[1];

      if(dfs(graph, u, v, new boolean[n+1])) {
        return edge;
      }
      graph.get(u).add(v);
      graph.get(v).add(u);
    }
    // 图的显示
    // display(graph);
    return new int[]{0, 0};
  }
  // u与v是否连通
  private boolean dfs(List<List<Integer>> graph, int u, int v, boolean[] visited){
    if(u == v) {
      return true;
    }
    visited[u] = true;
    for(Integer next : graph.get(u)) {
      if(!visited[next] && dfs(graph, next, v, visited)) {
        return true;
      }
    }
    return false;
  }

  private void display(List<List<Integer>> graph) {
    int n = graph.size();
    for(int i = 0; i < n; i++) {
      List<Integer> nextNodes = graph.get(i);
      System.out.println(i + ": " + nextNodes);
    }
  }
}
```


时间复杂度: 最坏和平均时间复杂度都是 O(|V|^2)。

空间复杂度: 存图空间 O(|V|+|E|) ， visited空间 O(|V|)，递归栈或队列空间 O(|V|) 。总体为线性空间复杂度 O(|V|+|E|)
