## 单串变形，需要考虑最后两个位置的情况

有一些单串问题在涉及状态时需要考虑最后两位的情况，因为只考虑最后一个的话无法对状态描述清楚

1. 最长的斐波那契子序列的长度：`dp[i][j]`:=  以 j, i 结尾，转移时在 [0..j] 中找满足条件的 k 这一步可以二分或哈希表
2. 最长等差数列 —— `dp[i][j]` := 以 j, i 结尾，转移时在 [0..j] 中找满足条件的 k 这一步用哈希表，键为数组值，值为保存下标的平衡树

### [873. Length of Longest Fibonacci Subsequence](https://leetcode.cn/problems/length-of-longest-fibonacci-subsequence/)

dp[i][j] 表示`nums[i]、nums[j]`作为`fibonacci subsequence`最后两个数字时，才能确定整个fibonacci子序列的最大长度。

$dp[j][i]= \begin{cases}\max (d p[k][j]+1,3), & 0 \leq k<j \\ 0, & k<0 \text { or } k \geq j\end{cases}$

N^3暴力求解：

```java
class Solution {
    public int lenLongestFibSubseq(int[] arr) {
      // dp[i][j] := 以arr[i]和arr[j]结尾的最长fabo子序列长度
      // dp[j][i] = dp[k][j] + 1 if(arr[k] + arr[j] == arr[i]) else 3
      int n = arr.length;
      int maxLen = 0;
      int[][] dp = new int[n][n];
      for(int i = 2; i < n; i++) {
        for(int j = 0; j < i; j++) {
          for(int k = 0; k < j; k++) {
            if(arr[k] + arr[j] == arr[i]) {
              dp[j][i] = Math.max(dp[k][j] + 1, 3);
              maxLen = Math.max(maxLen, dp[j][i]);
            }
          }
        }
      }
      return maxLen;
    }
}
```

优化为N^2

```java
class Solution {
  public int lenLongestFibSubseq(int[] arr) {
    // dp[i][j] := 以arr[i]和arr[j]结尾的最长fabo子序列长度
    // dp[j][i] = dp[k][j] + 1 if(arr[k] + arr[j] == arr[i]) else 3
    int n = arr.length;
    int maxLen = 0;
    int[][] dp = new int[n][n];
    for(int i = 2; i < n; i++) {
      Map<Integer, Integer> map = new HashMap<>();
      for(int j = 0; j < i; j++) {
        if(map.containsKey(arr[i] - arr[j])) {
          dp[j][i] = Math.max(dp[map.get(arr[i] - arr[j])][j] + 1, 3);
          maxLen = Math.max(maxLen, dp[j][i]);
        }
        map.put(arr[j], j);
      }
    }

    return maxLen;
  }
}
```

题中arr是单调严格递增的，所以还可以进一步优化：

```java
class Solution {
  public int lenLongestFibSubseq(int[] arr) {
    // dp[i][j] := 以arr[i]和arr[j]结尾的最长fabo子序列长度
    // dp[j][i] = dp[k][j] + 1 if(arr[k] + arr[j] == arr[i]) else 3
    int n = arr.length;
    int maxLen = 0;
    int[][] dp = new int[n][n];
    Map<Integer, Integer> map = new HashMap<>();
    map.put(arr[0], 0);
    map.put(arr[1], 1);
    for(int i = 2; i < n; i++) {
      // 因为数是严格递增的，所以arr[j]以及arr[i] - arr[j]一定不会是相等的，并且arr[j] * 2 > arr[i]保证了一次循环中map只会被利用一次
      for(int j = i - 1; j >= 0 && arr[j] * 2 > arr[i] ; j--) {
        if(map.containsKey(arr[i] - arr[j])) {
          dp[j][i] = Math.max(dp[map.get(arr[i] - arr[j])][j] + 1, 3);
          maxLen = Math.max(maxLen, dp[j][i]);
        }
      }
      map.put(arr[i], i);
    }
    return maxLen;
  }
}
```
