- Vamana 和 HNSW NSG 的区别  
	- both HNSW and NSG have no tunable parameter α and implicitly use α = 1. This is the main factor which lets Vamana achieve a better trade-off between graph degree and diameter  
	- while HNSW sets the candidate set V for the pruning procedure to be the final result-set of L candidates output by GreedySearch(s, p, 1, L), Vamana and NSG let V be the entire set of vertices visited by GreedySearch(s, p, 1, L)  
		- 这让 Vamana 和 NSG 有了一些更长距离的边 ; HNSW 因为没有长边，所以引入了多层的结构  
	- while NSG sets the starting graph to be an approximate K-nearest neighbor graph over the dataset, which is a time and memory intensive step, HNSW and Vamana have simpler initializations, with the former beginning with an empty graph and Vamana beginning with a random graph.   
	- Vamana makes two passes over the dataset, whereas both HNSW and NSG make only one pass, motivated by our observation that the second pass improves the graph quality.  
-  
- 具体实现 - 存储在 SSD 上的索引  
	- 需要解决内存太小产生的两个问题：  
		- how do we build a graph over a billion points,  
			- 聚类（有overlap） 分块建图 然后merge所有边到同一张图  
			- ？聚类 k-means 在磁盘上可以做好吗？  
		- how do we do distance comparisons between the query point and points in our candidate list at search time in Algorithm 1,  
	- Index layout  
		- 内存中读入所有点的压缩向量  
		- SSD中存储 graph 和对应的全精度向量 - 存储格式是对每个点 先存储其全精度向量，随后跟上R个邻节点ID；如果小于R个邻居则补零，这样可以通过id计算磁盘上的偏移位置  
	- 重要特性：  
		- Note that fetching a small number of random sectors from an SSD takes almost the same time as one sector  
	- BeamSearch  
		- 我的理解就是每次不止读一个节点（候选集中离目标向量最近）的邻居，而是读最近的W个邻居节点；减少round-trip时间  
-  
-  
- 实验结果  
	- vamana 相比于 NSG 和 HNSW 有提高 max degree 降低 hops 的趋势  
		- ![image.png](../assets/image_1699192918551_0.png)  
- 在 one-shot 和 merged vamana 建图的对比中  
	- one-shot 性能更好 可能是因为 merged vamana 的入度和出度都局限在 l/k=5% 的节点中，这样，可能会导致更多的跳数  
		- The single index outperforms the merged index  
	- 仍然是足够好的选择 仅比 single index 差了 20% 的 latency - 比其他方法更好  