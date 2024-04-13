## 前缀和简介

数组序列[a0, a1, ...., an]，前缀和定义为

$S_i=\sum_{j=0}^{j=i}a_j$

区间和定义
$R[left, right]=\sum_{i=left}^{i=right}a_j$


区间和和前缀和的联系为：
$R[left, right]=S_{right} - S_{left} + a_{left}$

前缀和的优势在于要求区间和时，可以用前缀和数组在O(1)的时间内推导出来，而不需要重新以O(N)的时间去计算一遍区间的和。

相当于前缀和已经缓存了区间和的结果，并且避免了重复的计算，这跟动态规划中的重复子问题是类似的。



注意在实际的 代码编写中，为了代码的简洁，我们总是以`preSum[i+1]`来表示`nums[0:i]`的和。

那么`nums[j:i]`的区间和 就为：`preSum[i + 1] - preSum[j]`。

### [303. 区域和检索 - 数组不可变](https://leetcode.cn/problems/range-sum-query-immutable/)


```java
class NumArray {
  int[] preSum;

  public NumArray(int[] nums) {
    preSum = new int[nums.length + 1];
    for(int i = 0; i < nums.length; i++) {
      preSum[i + 1] = preSum[i] + nums[i];
    }
  }

  public int sumRange(int left, int right) {
    return preSum[right + 1] - preSum[left];
  }
}
```

初始化时间复杂度O(n)，检索O(1)

空间复杂度O(n)
