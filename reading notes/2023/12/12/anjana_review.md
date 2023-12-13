C++中 set 集合 rbegin() 和 end() 区别

在 C++ 中，`set` 是一种有序的关联容器，而 `rbegin()` 和 `end()` 是 `set` 类中提供的成员函数，用于获取逆向迭代器和尾后迭代器（end 迭代器）。

1. **`rbegin()` 函数：**

   - `rbegin()` 函数返回一个逆向迭代器，指向集合的最后一个元素。逆向迭代器是一个反向遍历容器元素的迭代器。

   ```
   cppCopy code
   #include <iostream>
   #include <set>
   
   int main() {
       std::set<int> mySet = {1, 2, 3, 4, 5};
   
       auto rit = mySet.rbegin();  // 获取逆向迭代器
       while (rit != mySet.rend()) {
           std::cout << *rit << " ";
           ++rit;
       }
   
       return 0;
   }
   ```

   上面的代码使用 `rbegin()` 获取逆向迭代器，然后通过逆向迭代器遍历集合的元素。输出将是 `5 4 3 2 1`。

2. **`end()` 函数：**

   - `end()` 函数返回一个迭代器，指向集合的尾后位置。尾后位置是集合中最后一个元素的下一个位置。

   ```
   cppCopy code
   #include <iostream>
   #include <set>
   
   int main() {
       std::set<int> mySet = {1, 2, 3, 4, 5};
   
       auto it = mySet.begin();
       while (it != mySet.end()) {
           std::cout << *it << " ";
           ++it;
       }
   
       return 0;
   }
   ```

   上面的代码使用 `begin()` 获取迭代器，然后通过迭代器遍历集合的元素。输出将是 `1 2 3 4 5`。

**总结：**

- `rbegin()` 返回逆向迭代器，指向集合的最后一个元素。
- `end()` 返回迭代器，指向集合的尾后位置，即最后一个元素的下一个位置。

这两个函数都是用于在集合上进行迭代的，但一个是从尾向头遍历，另一个是从头向尾遍历。