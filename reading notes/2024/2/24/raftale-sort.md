## 插入排序

理解插入排序，可以想象成打扑克牌，每次抽一张牌都是插入到已经有序的牌中的适当位置。

在计算机的实现中，为了给插入的元素腾出空间，我们需要将其他所有元素在插入之前都向右移动一位。这种算法就叫做插入排序。

```java
public class InsertSort {
  // 升序
  public static void sort(int[] arr) {
    for(int i = 0; i < arr.length; i++) {
      for(int j = i; j > 0 && arr[j] < arr[j - 1]; j--) {
        swap(arr, j, j - 1);
      }
    }
  }

  private static void swap(int[] arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}
```

具体的时间复杂度是跟数据相关的，如果本身是有序的，那么只需要N-1次比较；如果是最坏条件下（逆序），时间复杂度为O(N^2)次比较和O(N^2)次交换。

平均时间复杂度为O(N^2)：O(N^4)次比较和O(N^4)次交换。

事实上，还有一个小优化，就是减少访问数组的次数，内循环中将较大的元素向右移动而不总是交换这两个元素。

```java
public class InsertSort {
  // 升序
  public static void sort(int[] arr) {
    for(int i = 0; i < arr.length; i++) {
      int currentNumber = arr[i];
      int j = i - 1;
      for(; j > 0 && currentNumber < arr[j]; j--) {
        arr[j + 1] = arr[j];
      }
      arr[j + 1] = currentNumber;
    }
  }
}
```
插入排序是稳定排序。
