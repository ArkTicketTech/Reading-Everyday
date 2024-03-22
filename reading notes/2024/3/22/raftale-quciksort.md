# 快速排序

快速排序是一种分治的排序算法。它将一个数组分成两个子数组，将两部分独立地排序。


pivot 是从数组中随机选择的一个数，称之为基数。

1. 遍历数组，比pivot小的数移到它的左边，比pivot大的数移到它的右边，这样pivot就将这个数组划分成了两个子数组。
2. 递归的实现上面的步骤，用归纳法证明是最终的结果一定是排序后的结果：如果左子数组和右子数组都是有序的，且切分元素在两者之间，那么最终的排序结果显而易见是有序的。

快速算法的代码：

第一步划分两个数组也就是快速排序的切分，假设选第一个数作为pivot，通过双指针，左指针遇到的比pivot大的与右指针遇到的比pivot小的进行交换，直到左指针与右指针相遇，然后与pivot交换，就能实现切分。

## 切分算法


切分策略：先固定一个pivot不动（一般是选择最左侧的元素`a[left]`），然后从数组的左端开始扫描直到找到一个大于等于它的元素，再从数组的右端开始向左扫描直到找到一个小于等于它的元素，然后交换它们的位置，这样就能保证做指针i的左侧元素都不大于切分元素，右指针j的右侧元素都不小于气氛元素。当这两个指针相遇时，我们只需要将切分元素`a[left]`和左子数组最右侧的元素`a[j]`交换然后返回`j`（pivot所在的新位置）即可。




时间复杂度：

快速排序的每一次遍历，都将基数摆到了最终位置上。第一轮遍历排好 1 个基数，第二轮遍历排好 2 个基数（如果基数选的不好，那就是排一个），第三轮遍历排好 4 个基数，因此总遍历次数为logN～N，每次遍历时间复杂度为O(n)，所以快速排序的时间复杂度为O(N*logN)~O(N2)，平均时间复杂度为O(N*logN)。

如果数组本身就是接近有序的，每次都选择第一个数的话，每次遍历就只会排出一个基数，此时时间复杂度就解决于O(N^2)了，因此一个思路就是打乱数组或者每次随机选择(代码中partition中的随机选一个pivot)，保证平均时间复杂度为O(N*logN)。

## [912. 排序数组](https://leetcode.cn/problems/sort-an-array/)

```java
class Solution {
  public int[] sortArray(int[] nums) {
    quickSort(nums);
    return nums;

  }

  public void quickSort(int[] nums) {
    quickSort(nums, 0, nums.length - 1);
  }

  public void quickSort(int[] nums, int start, int end) {
    if(start >= end) {
      return ;
    }
    int pivot = partition(nums, start, end);
    quickSort(nums, start, pivot - 1);
    quickSort(nums, pivot + 1, end);
  }

  public int partition(int[] nums, int start, int end) {
    // 随机选一个作为pivot, 防止每次选中最大/最小的pivot
    int random = new Random().nextInt(end - start + 1) + start;

    swap(nums, start, random);

    int pivot = nums[start];
    int left = start;
    int right = end + 1;
    while(true) {
      // 最左端向右移动找到第一个大于等于pivot的元素
      while(nums[++left] < pivot) {
        if(left == end) {
          break;
        }
      }
      // 最右端向左移动找到第一个小于等于pivot的元素
      while(nums[--right] > pivot) {
        if(right == start) {
          break;
        }
      }

      if(left >= right) {
        break;
      }
      swap(nums, left, right);
    } 

    swap(nums, start, right);
    return right;
  }

  private void swap(int[] nums, int i, int j) {
    int temp = nums[i];
    nums[i] = nums[j];
    nums[j] = temp;
  }
}
```
