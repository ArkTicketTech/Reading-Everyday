扫描线是用来求重叠区域的个数这类问题。

很经典的题就是求某一个时刻公交车上最多的人数，公交车的人上上下下，上车就+1，下车就-1，自然就能求出最多的人数。

这类问题基本上都是公交车问题的变种。

按时间排序也是求解问题的前提之一。

## [253. Meeting Rooms II](https://leetcode.cn/problems/meeting-rooms-ii/)

实际上就是在求重复区域的最大重叠数，思路可以用扫描线，排序后然后起点+1，终点-1，累计起来的就是答案。

```java
class Solution {
  public int minMeetingRooms(int[][] intervals) {
    // 实际上是在求重复区域的最大重叠数
    Arrays.sort(intervals, (a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] - b[1]);
    List<int[]> lines = new ArrayList<>();

    for (int i = 0; i < intervals.length; i++) {
      // 1 代表进入， -1 代表退出

      lines.add(new int[] { intervals[i][0], 1 });
      lines.add(new int[] { intervals[i][1], -1 });
    }
    // [[0,30],[5,10],[10,20]]
    // [0, 1] [5, 1] [10, -1] [10, 1] [20, -1] [30, -1]
    // 所以要保证先退出，再进入，否则出现这种[0, 1] [5, 1] [10, 1] [10, -1]就会比实际情况多一人
    Collections.sort(lines, (a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] - b[1]);
    int cnt = 0;
    int max = 0;
    for (int[] line : lines) {
      cnt += line[1];
      max = Math.max(max, cnt);
    }
    return max;
  }
}
```
