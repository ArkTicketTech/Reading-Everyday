# 冒泡排序

冒泡排序的核心思想是每一轮比较都将未排序中的最大值放到未排序的最后面去。

每一轮比较中，都将数组划分成了两个部分，一个是处在前面的未排序的部分，另一个是处在后面的排序好的部分。

每一轮比较中，不停的交换未排序部分中相邻的数，每次都将更大的数向后移动，结束后最大数必然能到最后。

上面说的是升序，降序道理是一样的。

```java
public class BubbleSort {
  // 升序
  public static void sort(int[] arr) {
    for(int i = 0; i < nums.length; i++) {
      for(int j = 0; j < nums.length - 1 - i; j++) {
        if(nums[j] > nums[j + 1]) {
          swap(nums, j, j + 1);
        }
      }
      // display(nums);
    }
  }

  private static void swap(int[] arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
  public void display(int[] nums) {
    for(int i = 0; i < nums.length; i++) {
      System.out.print(nums[i] + " ");
    }  
    System.out.println();
  }
}
```

时间复杂度为O(n^2)，空间复杂度为O(1)。

有个小优化点：如果当前轮次没有发生交换就表示已经有序的，不用再继续排序。
