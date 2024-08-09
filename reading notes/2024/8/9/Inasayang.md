优化掉边界检查

- 在循环中用迭代替换
  ```rust
    let arr = [1, 2, 3, 4, 5];
    for i in 0..arr.len() {
        println!("{}", arr[i]); // 有边界检查
    }
  ```
  ```rust
    let arr = [1, 2, 3, 4, 5];
    for &item in arr.iter() {
        println!("{}", item); // 不需要边界检查
    }
  ```

- 引到 Vec 时，先制作 Vec 的切片
  ```rust
    fn process_vector(vec: &Vec<i32>) {
        let slice = vec.as_slice();
        for &item in slice.iter() {
            println!("{}", item);
        }
    }
  ```
- 在索引变量的范围上添加断言
  ```rust
    fn process_vector(vec: &Vec<i32>) {
        for i in 0..vec.len() {
            assert!(i < vec.len()); // 添加断言
            let item = vec[i];
            println!("{}", item);
        }
    }
  ```
  ```rust
    fn process_vector(vec: &Vec<i32>) {
        for i in 0..vec.len() {
            debug_assert!(i < vec.len());// 只在 debug 模式下检查
            let item = vec[i];
            println!("{}", item);
        }
    }
  ```
- get_unchecked和get_unchecked_mut
 ```rust
    fn process_vector(vec: &Vec<i32>) {
        for i in 0..vec.len() {
            unsafe { // 使用 unsafe 块
                let item = vec.get_unchecked(i);
                println!("{}", item);
            }
        }
    }
 ```