## 拓扑排序

拓扑排序 (Topological Sorting) 是针对于有向无环图(DAG, Directed Acyclic Graph)的一种算法，它是对图中所有顶点按照先后顺序的一种线性排序。意思就是说如果图中的两个顶点u和v，想到达到v，必须先到达u，那么在拓扑排序中，u必须在v的前面。

更准确地，拓扑排序是对图的一种遍历，在这种遍历中，对一个顶点 v 的访问只发生在它所依赖的顶点被访问之后。

要理解拓扑排序，还需要搞清楚入度和初度的概念，某个顶点的「入度」为指向该顶点的边的数量，「出度」为该顶点指向其他顶点的边的数量。



1. 拓扑排序的起点必须是入度为0的顶点，也就是图中至少有一个顶点没有前置顶点；
2. 如果图中的所有的顶点的入度都大于0，就意味着所有顶点都至少有一个前置顶点，那么就没有顶点作为拓扑排序的起点。



拓扑排序的主要实现为基于 BFS 的 **Kahn算法** 以及基于 DFS的 **Tarjan拓扑排序算法**。

### kahn算法：

kahn算法的优势就是能实现线性时间的复杂度计算。

算法过程：

1. 构建邻接表graph；
2. 计算所有顶点的入度indegree，然后将入度为 0 的顶点放入队列中；
3. 开始BFS遍历：

   1. 每poll出一个当前顶点，加入到拓扑排序topSort中；
   2. 找出当前顶点的邻接顶点，入度减1；
   3. 遍历邻接顶点，如果发现有入度为0的，就加入队列。因为入度为0时说明所有指向该顶点的顶点都已经被标记了

1. bfs遍历完后，判断入度是否存在不为0的，有不为0的则说明图中有环；全部为0则输出拓扑排序。



时间复杂度O(V+E)

V表示顶点数，E表示边数

空间复杂度O(V+E)

#### [207. 课程表](https://leetcode.cn/problems/course-schedule/)

```java
class Solution {
  public boolean canFinish(int numCourses, int[][] prerequisites) {
    List<List<Integer>> graph = new ArrayList<>();
    for(int i = 0; i < numCourses; i++) {
      graph.add(new ArrayList<>());
    }
    int[] indegree = new int[numCourses];
    for(int[] prerequisite : prerequisites) {
      int u = prerequisite[1];
      int v = prerequisite[0];
      graph.get(u).add(v);
      indegree[v]++;
    }
    Deque<Integer> queue = new ArrayDeque<>();
    for(int i = 0; i < indegree.length; i++) {
      if(indegree[i] == 0) {
        queue.offer(i);
      }
    }

    while(!queue.isEmpty()) {
      int u = queue.poll();
      for(Integer v : graph.get(u)) {
        indegree[v]--;
        if(indegree[v] == 0) {
          queue.offer(v);
        }
      }
    }

    for(int i = 0; i < indegree.length; i++) {
      if(indegree[i] != 0) {
        return false;
      }
    }
    return true;
  }
}
```
