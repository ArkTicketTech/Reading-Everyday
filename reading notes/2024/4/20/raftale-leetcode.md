刷了一些回溯的题。
回溯算法是一种通过不断 尝试 ，搜索一个问题的一个解或者 所有解 的方法。在求解的过程中，如果继续求解不能得到题目要求的结果，就需要 回退 到上一步尝试新的求解路径。回溯算法的核心思想是：在一棵 隐式的树（看不见的树） 上进行一次深度优先遍历。


## [17. 电话号码的字母组合](https://leetcode.cn/problems/letter-combinations-of-a-phone-number/)

```java
class Solution {
  public List<String> letterCombinations(String digits) {
    if(digits.length() == 0) {
      return new ArrayList<>();
    }
    Map<Character, String> phoneMap = new HashMap<>() {{
      put('2', "abc");
      put('3', "def");
      put('4', "ghi");
      put('5', "jkl");
      put('6', "mno");
      put('7', "pqrs");
      put('8', "tuv");
      put('9', "wxyz");
    }};

    List<String> result = new ArrayList<>();
    dfs(digits, 0, new StringBuilder(), result, phoneMap);
    return result;
  }
  private void dfs(String digits, int start, StringBuilder sb, List<String> result, Map<Character, String> phoneMap) {
    if(start == digits.length()) {
      result.add(sb.toString());
      return;
    }
    String str = phoneMap.get(digits.charAt(start));
    if(str == null) {
      return;
    }
    for(int i = 0; i < str.length(); i++) {
      sb.append(str.charAt(i));
      dfs(digits, start + 1, sb, result, phoneMap);
      sb.deleteCharAt(sb.length() - 1);
    }
  }
}
```
