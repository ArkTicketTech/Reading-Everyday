## 定义

维基百科：优先队列是计算机科学中的一类抽象数据类型。优先队列中的每个元素都有各自的优先级，优先级最高的元素最先得到服务；优先级相同的元素按照其在优先队列中的顺序得到服务。

队列弹出的元素是有优先级的，如始终弹出的是队列中最小的元素或者最大的元素。

优先队列的实现是通过最大堆和最小堆来实现的，堆的逻辑结构是一种完全二叉树。

完全二叉树只允许最后一层不满，且最后一层必须从左往右排序，最优一行元素之间不能有间隔



堆是一种数据结构，而优先队列是一种抽象的数据结构，堆并不是优先队列，堆是实现优先队列的一种方式。

实现优先队列的方式有很多种，比如数组和链表。但是这些实现方式只能保住插入操作和删除操作中的一种能在O(1)的时间复杂度在O(1)，而另一种则是在O(N)。但是堆可以使插入和删除操作的时间复杂度为O(logN)。

最大堆：最大元素始终在堆顶

最小堆：最小元素始终在堆顶



## 初级实现

1. 数组实现（无序）：数组无序，删除最大元素时参考选择排序的内循环，我们可以将最大元素和边界元素进行交换然后它，和我们对栈的pop()方法的实现一样；
2. 数组实现（有序）：使用插入排序保持数组有序，删除最大元素就是删除边界元素
3. 链表表示法：同理



## 堆的定义

堆是一种特别的二叉树，满足以下条件的二叉树，可以称之为堆：

1. 完全二叉树
2. 每一个节点的值都必须大于等于或者小于等于其孩子节点的值

## 堆的实现

插入：找到第一个空子节点，插入，然后与父节点不断交换，直到满足特点

删除堆顶元素：找到最后一个子节点，替换根上的元素（删除），然后与子节点不断替换，直到符合特点

```java
//「最大堆」的实现
public class MaxHeap {

  // 使用数组创建完全二叉树的结构，然后使用二叉树构建一个「堆」
  int[] maxHeap;
  // heapSize用于数组的大小，因为数组在创建的时候至少需要指明数组的元素个数
  int heapSize;
  // realSize用于记录「堆」的元素个数
  int realSize = 0;

  public MaxHeap(int heapSize) {
    this.heapSize = heapSize;
    maxHeap = new int[heapSize + 1];
    // 为了便于完全二叉树的管理，数组的第0个索引的元素我们不会使用，请随便设置设置成任何一个值。
    maxHeap[0] = 0;
  }


  // 添加元素函数
  public void add(int element) {
    realSize++;
    // 如果「堆」中元素的个数大于一开始设定的数组的个数，则返回「Add too many elements」
    if (realSize > heapSize) {
      System.out.println("Add too many elements!");
      realSize--;
      return;
    }
    // 将添加的元素添加到数组中
    maxHeap[realSize] = element;
    // 新增元素的索引位置
    int index = realSize;
    // 新增元素的父节点的索引位置
    // 注意，如果用数组表示完全二叉树，并且根结点存储在数组的索引1的位置的时候，任何一个节点的父节点索引位置为「该节点的索引位置/2」，任何一个节点的左孩子节点的索引位置为「该节点的索引位置*2」，任何一个节点的右孩子节点的索引位置为「该节点的索引位置*2+1」
    int parent = index / 2;
    // 当添加的元素大于父节点时，需要将父节点的值和新增元素的值交换
    while (maxHeap[index] > maxHeap[parent] && index > 1) {
      swap(maxHeap, index, parent);
      index = parent;
      parent = index / 2;
    }
  }

  // 获取堆顶元素函数
  public int peek() {
    return maxHeap[1];
  }

  // 删除堆顶元素函数
  public int pop() {
    // 如果当前「堆」的元素个数为0， 则返回「Don't have any element」
    if (realSize < 1) {
      System.out.println("Don't have any element!");
      return Integer.MIN_VALUE;
    }
    int removeElement = maxHeap[1];
    // 将「堆」中的最后一个元素赋值给堆顶元素
    maxHeap[1] = maxHeap[realSize];
    realSize--;
    int index = 1;
    // 当删除的元素不是孩子节点时
    while (index < realSize && index <= realSize / 2) {
      // 被删除节点的左孩子节点
      int left = index * 2;
      // 被删除节点的右孩子节点
      int right = (index * 2) + 1;
      // 当删除节点的元素小于 左孩子节点或者右孩子节点，代表该元素的值小，此时需要将该元素与左、右孩子节点中最大的值进行交换
      if (maxHeap[index] >= maxHeap[left] && maxHeap[index] >= maxHeap[right]) {
        break;
      }
      if (maxHeap[left] > maxHeap[right]) {
        swap(maxHeap, index, left);
        index = left;
      } else {
        swap(maxHeap, index, right);
        index = right;
      }
    }
    return removeElement;

  }

  // 返回「堆」的元素个数
  public int size() {
    return realSize;
  }

  public void swap(int[] nums, int i, int j) {
    int temp = nums[i];
    nums[i] = nums[j];
    nums[j] = temp;
  }

  public String toString() {
    if (realSize == 0) {
      return "No element!";
    } else {
      StringBuilder sb = new StringBuilder();
      sb.append('[');
      for (int i = 1; i <= realSize; i++) {
        sb.append(maxHeap[i]);
        sb.append(',');
      }
      sb.deleteCharAt(sb.length() - 1);
      sb.append(']');
      return sb.toString();
    }
  }

  public static void main(String[] args) {
    // 测试用例
    MaxHeap maxheap = new MaxHeap(5);
    maxheap.add(1);
    maxheap.add(2);
    maxheap.add(3);
    // [3,1,2]
    System.out.println(maxheap);
    // 3
    System.out.println(maxheap.peek());
    // 3
    System.out.println(maxheap.pop());
    System.out.println(maxheap.pop());
    System.out.println(maxheap.pop());
    // No element
    System.out.println(maxheap);
    maxheap.add(4);
    // Add too many elements
    maxheap.add(5);
    System.out.println(maxheap);
  }


}
```
