- ## 0X05 排序  
	- 中位数  
		- 如何动态维护中位数？ #card #Algorithm  
			- 在线 - 对顶堆  
			- 离线 - 链表+hash  
	- 第K大数  
		- 快速选择法  
	- 逆序对  
		- 归并排序  
		- 树状数组  
- ## 0X06 倍增  
	- RMQ问题 - ST算法  
		- 令 $$F[i, j]$$ 表示 $$[i, i + 2^j - 1]$$ 范围内最大的值  
		-  
		  ``` cpp
		  		  void ST_prework() {
		  		    for (int i = 0; i <= n; i++) f[i][0] = a[i];
		  		    
		  		    int t = log(n) / log(2) + 1;
		  		    for (int j = 1; j < t; j++) {
		  		      for (int i = 1; i <= n - (1 << j) + 1; i++) {
		  		        f[i][j] = max(f[i][j-1], f[i + (1 << (j-1))][j-1]);
		  		      }
		  		    }
		  		  }
		  		  
		  		  // 2^k < r - l + 1 <= 2^(k+1)
		  		  int ST_query(int l, int r) {
		  		    int k = log(r - l + 1) / log(2);
		  		    return max(f[l][k], f[r - (1 << k) + 1][k]);
		  		  }
		  ```
	-  