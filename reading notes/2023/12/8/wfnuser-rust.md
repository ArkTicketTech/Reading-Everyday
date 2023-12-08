- 2.12 包和模块
	- 将大规模工程的代码写到一个或者几个文件中，会有一些问题
		- 单个文件过大，打开缓慢
		- 查询和定位效率降低
		- 只有一个代码层次将是一个灾难
		- 容易导致 bug
	- rust对代码的组织管理包括：
		- 项目(Packages)：一个 `Cargo` 提供的 `feature`，可以用来构建、测试和分享包
		- 包(Crate)：一个由多个模块组成的树形结构，可以作为三方库进行分发，也可以生成可执行文件进行运行
		- 模块(Module)：可以一个文件多个模块，也可以一个文件一个模块，模块可以被认为是真实项目中的代码组织单元
	- 包和Package
		- Package 和 Crate 很容易被搞混
		- Crate
			- 是一个独立的可编译单元，编译后会生成一个可执行文件或者库；将相关联的功能打包在一起，便于多个项目分享
			- 比如 `use rand;`  就引入了 rand 包
		- Package - 项目 （这是一个特别的命名）
			- 由于 `Package` 就是一个项目，因此它包含有独立的 `Cargo.toml` 文件，以及因为功能性被组织在一起的一个或多个包。一个 `Package` 只能包含**一个**库(library)类型的包，但是可以包含**多个**二进制可执行类型的包。
			- 二进制 Package
				-
				  ```rust
				  $ cargo new my-project
				       Created binary (application) `my-project` package
				  $ ls my-project
				  Cargo.toml
				  src
				  $ ls my-project/src
				  main.rs
				  ```
				- `cargo run` 可以运行该项目； 根文件是 src/main.rs
			- 库 Package
				-
				  ```rust
				  $ cargo new my-lib --lib
				       Created library `my-lib` package
				  $ ls my-lib
				  Cargo.toml
				  src
				  $ ls my-lib/src
				  lib.rs
				  ```
				- 该项目无法独立运行，必须被其他项目引用才可以运行； 根文件是 src/lib.rs
		- Module 模块
			- 创建嵌套模块
				-
				  ```rust
				  // 餐厅前厅，用于吃饭
				  mod front_of_house {
				      mod hosting {
				          fn add_to_waitlist() {}
				  
				          fn seat_at_table() {}
				      }
				  
				      mod serving {
				          fn take_order() {}
				  
				          fn serve_order() {}
				  
				          fn take_payment() {}
				      }
				  }
				  ```
			- 对应的模块树
				-
				  ```
				  crate
				   └── front_of_house
				       ├── hosting
				       │   ├── add_to_waitlist
				       │   └── seat_at_table
				       └── serving
				           ├── take_order
				           ├── serve_order
				           └── take_payment
				  ```
			- 引用路径
				-
				  ```rust
				  mod front_of_house {
				      mod hosting {
				          fn add_to_waitlist() {}
				      }
				  }
				  
				  pub fn eat_at_restaurant() {
				      // 绝对路径
				      crate::front_of_house::hosting::add_to_waitlist();
				  
				      // 相对路径
				      front_of_house::hosting::add_to_waitlist();
				  }
				  ```
			- 可见性
				- 这样是不行的
					-
					  ```rust
					  mod front_of_house {
					      mod hosting {
					          fn add_to_waitlist() {}
					      }
					  }
					  
					  pub fn eat_at_restaurant() {
					      // 绝对路径
					      crate::front_of_house::hosting::add_to_waitlist();
					  
					      // 相对路径
					      front_of_house::hosting::add_to_waitlist();
					  }
					  
					  ```
				- 需要设置为 pub
					-
					  ```rust
					  mod front_of_house {
					      pub mod hosting {
					          pub fn add_to_waitlist() {}
					      }
					  }
					  ```
			- super 引用模块
				-
				  ```rust
				  fn serve_order() {}
				  
				  // 厨房模块
				  mod back_of_house {
				      fn fix_incorrect_order() {
				          cook_order();
				          super::serve_order();
				      }
				  
				      fn cook_order() {}
				  }
				  
				  ```
			- self - 其实可以省略
				-
				  ```rust
				  fn serve_order() {
				      self::back_of_house::cook_order()
				  }
				  
				  mod back_of_house {
				      fn fix_incorrect_order() {
				          cook_order();
				          crate::serve_order();
				      }
				  
				      pub fn cook_order() {}
				  }
				  
				  ```
			-