重读《算法竞赛入门经典》。

分治法思考步骤：

1. 划分问题：问题实例划分成子问题。
2. 递归求解：递归解决各个子问题。
3. 合并问题：合并子问题得到原问题解。

应用于排序和查找：

归并排序：

1. 把序列分成元素个数尽量相等的两半。
2. 把两半元素分别排序。
3. 把两个有序表合并成一个。

```go
func merge_sort(nums []int, int x, int y, t []int) {
    if (y - x > 1) {
        m := x + (y - x) / 2
        p, q, i = x, m, x
        merge_sort(nums, x, m, t)
        merge_sort(nums, m, y t)
        for ;p < m || q < y; {
            if q >= y || (p < m && nums[p] <= nums[q]) {
                t[i] = nums[p]
                i = i + 1
                p = p + 1
            } else {
                t[i] = nums[q]
                i = i + 1
                q = q + 1
            }
        }
        for i = x; i < y; i++) nums[i] = T[i]
    }
}
```

二分查找：通过排序得到有序数列，为检索带来方便。

1. 把原序列划分成元素个数尽量接近的两个子序列。
2. 递归查找。

```go
func bsearch(nums []int, int x, int y, int v) int {
  var m int
  for ;x < y; {
      m = x + (y - x) / 2;
      if nums[m] == v) {
          return m
      } else if nums[m] > v {
          y = m
      } else {
          x = m + 1
      }
  }
  return -1
}
```
