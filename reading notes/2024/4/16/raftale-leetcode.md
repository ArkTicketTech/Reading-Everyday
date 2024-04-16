二分查找是能够在O(logN)找到目标值。

二分的核心是在有序的条件后不断夹逼求解空间，核心是求解空间 和 不断夹逼。

## 循环不变量

二分查找本质上可以看成一个数学归纳法，不断的排除不可靠元素，不断的收敛区间。每一次的开始都基于上一次的结束。

顾名思义，循环不变量是在循环的过程中保持不变的性质。

循环不变量就是定义一个状态或者性质，使其在接下来的不断循环中，这个定义始终保持不变。

《算法导论（第 3 版）》对于循环不变量的描述是这样的：

循环不变式主要用来帮助我们理解算法的正确性。关于循环不变式，我们必须证明三条性质：

**初始化**：循环的第一次迭代之前，它为真。

**保持**：如果循环的某次迭代之前它为真，那么下次迭代之前它仍为真。

**终止**：在循环终止时，不变式为我们提供一个有用的性质，该性质有助于证明算法是正确的。





### [34. Find First and Last Position of Element in Sorted Array](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/)

找到第一个和最后一个目标元素。

问题转化为：找到第一个大于等于 target 的元素。

int left = 0, right = len - 1;

int mid = left + (right - left) / 2;

nums[mid] > target: right = mid - 1;

nums[mid] == target: right = mid - 1;

nums[mid] < target: left = mid + 1;

循环不变量：left - 1 一定是小于 target 的，right+1 一定是大于等于 target 的。



循环不变量：

L-1 始终是小于 target 的，R+1 始终是大于 target 的



```java
class Solution {
  public int[] searchRange(int[] nums, int target) {
    int start = lowerBound(nums, target); // 选择其中一种写法即可
    if (start == nums.length || nums[start] != target)
      return new int[]{-1, -1};
    // 如果 start 存在，那么 end 必定存在
    int end = lowerBound(nums, target + 1) - 1;
    return new int[]{start, end};
  }
  /**
  * return the first num greater than target
  * [left, right]
  * */
  public int findFirstGreaterThan_leftRightClose(int[] nums, int target) {
    int left = 0; 
    int right = nums.length - 1;
    // [left, right], 保证区间不为空。当left > righ时， 区间就为空， 如[5, 5] 代表5， [5, 4] 代表空
    while(left <= right) {
      int mid = left + (right - left) / 2;
      // 循环不变量: 
      // nums[left - 1] 始终小于 target
      // nums[right + 1] 始终大于等于 target
      // 因此 right + 1 or left 就是第一个大于target的数
      if(nums[mid] < target) {
        // [mid + 1, right]. 保证nums[left - 1] 始终小于 target
        left = mid + 1;
      } else {
        // [left, mid - 1]。保证nums[right + 1] 始终大于等于 target
        right = mid - 1;
      }
    }
    // or return right + 1
    return left;
  }
}
```

边界：

1. 如果所有的数都小于target，那么left是一直向右移动的，移动到nums.length位置；
2. 如果所有的数都大于target，那么right是一直向左移动的，直到移动到0的位置。
