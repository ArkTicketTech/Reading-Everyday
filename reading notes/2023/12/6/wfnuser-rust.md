- 2.11 返回值和错误处理
	- rust的错误分为两类：
		- 可恢复错误 - Result<T, E>
		- 不可恢复错误 - panic!
	- 错误传播
		- ? 宏
			-
			  ```rust
			  use std::fs::File;
			  use std::io;
			  use std::io::Read;
			  
			  fn read_username_from_file() -> Result<String, io::Error> {
			      let mut f = File::open("hello.txt")?;
			      let mut s = String::new();
			      f.read_to_string(&mut s)?;
			      Ok(s)
			  }
			  ```
			- 等价于
			-
			  ```rust
			  let mut f = match f {
			      // 打开文件成功，将file句柄赋值给f
			      Ok(file) => file,
			      // 打开文件失败，将错误返回(向上传播)
			      Err(e) => return Err(e),
			  };
			  ```
			- 且 ? 可以自动进行错误类型提升
			-
			  ```rust
			  fn open_file() -> Result<File, Box<dyn std::error::Error>> {
			      let mut f = File::open("hello.txt")?;
			      Ok(f)
			  }
			  
			  // 错误从 std::io::Error 在 ? 下自动转化成了 std::error::Error
			  ```