**单调队列**(Monotonic Queue)是一种特殊的队列，它首先是一个队列，其次队列内部的元素**单调递增**(递增队列)或者**单调递减**(递减队列)。

注意：单调队列在算法应用中大多是基于**双端队列**(Deque)实现的，因为经常会涉及其他操作，而不是单单维护一个单调队列。

https://zhuanlan.zhihu.com/p/346354943

基本思想：

**单调队列**是一种主要用于解决**滑动窗口**类问题的数据结构

```java
class MonotonicQueue {
    // 在队尾添加元素 n
    void push(int n);
    // 返回当前队列中的最大值
    int max()
    // 队列元素如果是n，删除它
    void pop(int n);
}
```

单调队列的核心思路是push时要把队列中比新元素小的元素都删除掉，这样push就能够保证队列的元素是单调递减的。

借助双端队列：

```java
public interface Deque<E> extends Queue<E> {
  // 在队头插入元素 n
  void offerFirst(int n);
  // 在队尾插入元素 n
  void offerLast(int n);
  // 在队头删除元素
  E pollFirst();
  // 在队尾删除元素
  E pollLast();
  // 返回队头元素
  int peekFirst();
  // 返回队尾元素
  int peekLast();
}
```

push时要把队列中比新元素小的元素都删除掉

```java
class MonotonicQueue {
  Deque<Integer> queue= new LinkedList<>();
  // push时始终保证队列是单调递减的
  void push(int n) {
    while(!queue.isEmpty() && queue.peekLast() < n) {
      queue.pollLast();
    }
    queue.offer(n);
  }
  // 最大值即队首元素
  int max() {
    return queue.peek();
  }

  //如果n是队首元素，移除 
  void pop(int n) {
    if (!queue.isEmpty() && queue.peek() == n) {
      queue.poll();
    }

  }
}
```

### [239. Sliding Window Maximum](https://leetcode.cn/problems/sliding-window-maximum/)

暴力做法，就是每一次都遍历滑动窗口找最大值，时间复杂度为O(n*k)。

遍历滑动窗口有优化的点，比如当`nums = [1, 2, 4, 3, 5], k = 3`，首先窗口是`[1, 2, 4]`，但实际上因为滑动窗口在向右移动，且4最大，所以`[1, 2]`是无用的；同理，遇到5时，比5小的数也没有了。整个过程滑动窗口就表现的像一个单调队列，队列中始终是单调递减的。

需要注意的是：队列记录的是下标，每次滑动窗口向右移动时，如果队首的下标不在窗口内，就需要弹出去。



构建单调队列我们可以利用双端队列。

```java
class Solution {
  public int[] maxSlidingWindow(int[] nums, int k) {
    int n = nums.length;
    Deque<Integer> deque = new ArrayDeque<>();
    for(int i = 0; i < k; i++) {
      while(!deque.isEmpty() && nums[i] >= nums[deque.peekLast()]) {
        deque.pollLast();
      }
      deque.offerLast(i);
    }
    int[] ans = new int[n - k + 1];
    ans[0] = nums[deque.peekFirst()];
    for(int i = k; i < n; i++) {
      while(!deque.isEmpty() && nums[i] >= nums[deque.peekLast()]) {
        deque.pollLast();
      }
      deque.offerLast(i);
      // [0, 1, 2, 3, 4]
      if(deque.peekFirst() <= i - k) {
        deque.pollFirst();
      }
      ans[i - k + 1] = nums[deque.peekFirst()];
    }
    return ans;

  }
}
```

