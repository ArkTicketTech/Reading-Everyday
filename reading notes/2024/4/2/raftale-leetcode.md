最近在刷leetcode，三数之和中，对于重复数字的细节处理了很久， 
两数之和的重复数字可以用set去重，三个数时就需要排序然后不处理重复的数字，贴段代码吧。
```java
class Solution {
  public List<List<Integer>> threeSum(int[] nums) {
    Arrays.sort(nums);
    List<List<Integer>> results = new ArrayList<>();
    int len = nums.length;
    // 固定一个数，剩下选择两个数，让问题变成two-sum
    for(int i = 0; i < len - 2; i++) {
      // 防止出现重复的数
      if(i != 0 && nums[i] == nums[i-1]) {
        continue;
      }
      // 如果前三个数之和大于0，后续不可能有大于0的情况了
      if(nums[i] + nums[i+1] + nums[i+2] > 0) {
        break;
      }
      // 如果这个数 和最后两个数的和小于0，说明nums[i]太小，继续下一个数
      if(nums[i] + nums[len-2] + nums[len-1] < 0) {
        continue;
      }
      List<List<Integer>> result = twoSum(nums, i + 1, -nums[i]);
      for(List<Integer> list : result) {
        results.add(Arrays.asList(nums[i], list.get(0), list.get(1)));
      }
    }

    return results;
  }
  // 两数之和 nums必须是排好序的
  // 返回结果不能是重复的
  public List<List<Integer>> twoSum(int[] nums, int start, int target) {

    Set<Integer> set = new HashSet<>();
    List<List<Integer>> result = new ArrayList<>();
    Set<Integer> duplicate = new HashSet<>();
    for(int i = start; i < nums.length; i++) {
      // 防止出现重复结果
      if(duplicate.contains(nums[i])) {
        continue;
      }
      if(set.contains(target - nums[i])) {
        duplicate.add(nums[i]);
        result.add(Arrays.asList(target - nums[i], nums[i]));
      }
      set.add(nums[i]);
    }
    return result;
  }
}
```
