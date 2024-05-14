满足单调性的栈，插入元素时，在保证该元素插入到栈顶后整个栈满足单调性的前提下弹出最少的元素。

例如，栈中自顶向下的元素为 

插入元素14时为了保证单调性需要依次弹出元素0、11，操作后栈变为 

单调栈比朴素的栈更为严格，它要求每次入栈的元素必须要有序，如果新元素入栈不符合要求，则将栈顶元素不断出栈，直到最新的元素满足单调的要求，使之形成单调递增/单调递减的一个栈。

这个保证单调性的原则是通过不断遗弃老的元素达成的，在这个遗弃的过程中就可以做一些事情，比如计算他们的差值，如[739. 每日温度](https://leetcode.cn/problems/daily-temperatures/); 或是找出下一个更大的值，如496。

```java
public void insert(int x) {
  while(!stack.isEmpty() && stack.peek() < x) {
  	stack.pop();
	}
  // do something
	stack.push(x);
}
```

为了方便描述，如果出栈的元素是单调递减的，那我们称这个栈为单调递减栈，也就是说，单调递减栈是从栈顶开始递减的；

如果出栈的元素是单调递增的，那我们称这个栈为单调递增栈，也就是说，单调递增栈是从栈顶开始递增的。

## [496. Next Greater Element I](https://leetcode.cn/problems/next-greater-element-i/)

### 暴力法

直接hashmap暴力求解

```java
class Solution {
  public int[] nextGreaterElement(int[] nums1, int[] nums2) {
    int len1 = nums1.length;
    int len2 = nums2.length;
    int[] res = new int[len1];
    Map<Integer, Integer> numToIndexMap = new HashMap<>();
    for(int i = 0; i < len2; i++) {
      numToIndexMap.put(nums2[i], i);
    }
    for(int i = 0; i < len1; i++) {
      int num = nums1[i];
      int index = numToIndexMap.get(num);
      boolean flag = false;
      for(int j = index + 1; j < len2; j++) {
        if(nums2[j] > num) {
          flag = true;
          res[i] = nums2[j];
          break;
        }
      }
      if(!flag) {
        res[i] = -1;
      }
    }

    return res;

  }
}
```

时间复杂度O(m*n)。

### 单调栈

因为nums1是nums2的子集，所以这个问题可以认为是找出nums2中每个元素的下一个最大元素是什么？

定位nums1的数在nums2的位置可以用HashMap来快速定位



对于`[1, 2, 3, 4, 5]`暴力的话，时间复杂度实际上就是`O(N)`，但对于`[4, 3, 2, 1, 5]`暴力时，时间复杂度就是`O(N^2)`，实际上在找4的下一个更大元素时，就已经知道了`[3, 2, 1]`的下一个更大元素了，也就是说一次遍历到5的时候，前面4个数的答案就可以知道了。由于一次遍历到5的时候不可能什么都不记录就知道前面数的答案，所以需要一个数据结构来存储，在遇到5的时候能让前面的数得到一个释放，有先进先出的特性那就自然是栈比较合适。并且这个栈的特性是**从栈顶到栈底单调递增的栈。**

```java
class Solution {
  public int[] nextGreaterElement(int[] nums1, int[] nums2) {
    // 1. nums1 是 nums2的子集 => 求nums2中下一个更大的元素
    // 从前向后找时间复杂度是O(n^2)，从后向前找
    // [1, 2, 3, 4]
    Deque<Integer> stack = new ArrayDeque<>();
    Map<Integer, Integer> map = new HashMap<>();
    // [4, 3, 2, 1, 5, 6, 5, 4, 3]
    for(int i = 0; i < nums2.length; i++) {
      // 从栈顶到栈底单调递增的栈
      while(!stack.isEmpty() && stack.peek() < nums2[i]) {
        int num = stack.pop();
        map.put(num, nums2[i]);
      }
      stack.push(nums2[i]);
    }
    // 遍历完之后仍然在栈中的说明现在
    while(!stack.isEmpty()) {
      map.put(stack.pop(), -1);
    }

    int[] ans = new int[nums1.length];
    for(int i = 0; i < nums1.length; i++) {
      ans[i] = map.get(nums1[i]);
    }
    return ans;
  }
}
```

这种栈由于其单调性，也就是我们前面说到的单调栈，本质上还是为了解决重复遍历的问题。



事实上，我们还可以从后向前遍历，`[3, 6, 4, 2, 3, 5]`从后向前遍历，一次遍历到4时就知道`[4, 2, 1]`的下一个更大元素是5，但在这中间我们还知道2的下一个元素是3，也就是在向前遍历的过程中，比5小的元素`[3, 2]`也是要存储起来的，直到遇到了更大的元素`4`，`[3, 2]`的存储从元素才没有了价值，因为更前面的元素的下一个更大的元素必然是刚遇到的这个元素。所以还是一个**从栈顶到栈底单调递增的栈存储**。

我们如何知道2的下一个更大的元素是2呢，就是在将2弹入栈之前。

```java
class Solution {
  public int[] nextGreaterElement(int[] nums1, int[] nums2) {
    // 1. nums1 是 nums2的子集 => 求nums2中下一个更大的元素
    // 从前向后找时间复杂度是O(n^2)，从后向前找
    // [1, 2, 3, 4]
    Deque<Integer> stack = new ArrayDeque<>();
    Map<Integer, Integer> map = new HashMap<>();
    // [3, 6, 4, 2, 3, 5]
    for(int i = nums2.length - 1; i >= 0; i--) {
      // 从栈顶到栈底单调递增的栈
      // 遇到更小的元素就存储起来，遇到更大的元素就弹出来
      while(!stack.isEmpty() && stack.peek() < nums2[i]) {
        stack.pop();
      }
      if(stack.isEmpty()) {
        map.put(nums2[i], -1);
      } else {
        map.put(nums2[i], stack.peek());
      } 
      stack.push(nums2[i]);
    }

    int[] ans = new int[nums1.length];
    for(int i = 0; i < nums1.length; i++) {
      ans[i] = map.get(nums1[i]);
    }
    return ans;
  }
}
```

time complexity: O(nums1.length + nums2.length)

space complexity: O(nums2.length)
