- #Rust语言圣经 #Rust基础入门  
	- 一些重要概念：  
		- 所有权  
		- 宏编程  
		- 模式匹配  
	-  
	  ``` rust
	  	  // Rust 程序入口函数，跟其它语言一样，都是 main，该函数目前无返回值
	  	  fn main() {
	  	      // 使用let来声明变量，进行绑定，a是不可变的
	  	      // 此处没有指定a的类型，编译器会默认根据a的值为a推断类型：i32，有符号32位整数
	  	      // 语句的末尾必须以分号结尾
	  	      let a = 10;
	  	      // 主动指定b的类型为i32
	  	      let b: i32 = 20;
	  	      // 这里有两点值得注意：
	  	      // 1. 可以在数值中带上类型:30i32表示数值是30，类型是i32
	  	      // 2. c是可变的，mut是mutable的缩写
	  	      let mut c = 30i32;
	  	      // 还能在数值和类型中间添加一个下划线，让可读性更好
	  	      let d = 30_i32;
	  	      // 跟其它语言一样，可以使用一个函数的返回值来作为另一个函数的参数
	  	      let e = add(add(a, b), add(c, d));
	  	  
	  	      // println!是宏调用，看起来像是函数但是它返回的是宏定义的代码块
	  	      // 该函数将指定的格式化字符串输出到标准输出中(控制台)
	  	      // {}是占位符，在具体执行过程中，会把e的值代入进来
	  	      println!("( a + b ) + ( c + d ) = {}", e);
	  	  }
	  	  
	  	  // 定义一个函数，输入两个i32类型的32位有符号整数，返回它们的和
	  	  fn add(i: i32, j: i32) -> i32 {
	  	      // 返回相加值，这里可以省略return
	  	      i + j
	  	  }
	  ```
	- 2.1 变量绑定与解构  
		- 为何手动设置变量的可变性？  
			- 不可变性可以为编程提供安全性；且避免一些不必要的 runtime 检查，提高性能  
		- 赋值 vs 绑定  
			- 绑定 - 更加体现了 rust 中的所有权概念，对象之前的主人将会在绑定之后丧失对该对象的所有权  
		- 使用下划线开头忽略未使用的变量  
			-  
			  ``` rust
			  			  fn main() {
			  			      let _x = 5;
			  			      let y = 10;
			  			  }
			  			  
			  			  warning: unused variable: `y`
			  			   --> src/main.rs:3:9
			  			    |
			  			  3 |     let y = 10;
			  			    |         ^ help: 如果 y 故意不被使用，请添加一个下划线前缀: `_y`
			  			    |
			  			    = note: `#[warn(unused_variables)]` on by default
			  ```
		- 变量解构  
			-  
			  ``` rust
			  			  fn main() {
			  			      let (a, mut b): (bool,bool) = (true, false);
			  			      // a = true,不可变; b = false，可变
			  			      println!("a = {:?}, b = {:?}", a, b);
			  			  
			  			      b = true;
			  			      assert_eq!(a, b);
			  			  }
			  			  
			  			  struct Struct {
			  			      e: i32
			  			  }
			  			  
			  			  fn main() {
			  			      let (a, b, c, d, e);
			  			  
			  			      (a, b) = (1, 2);
			  			      // _ 代表匹配一个值，但是我们不关心具体的值是什么，因此没有使用一个变量名而是使用了 _
			  			      [c, .., d, _] = [1, 2, 3, 4, 5];
			  			      Struct { e, .. } = Struct { e: 5 };
			  			  
			  			      assert_eq!([1, 2, 1, 4, 5], [a, b, c, d, e]);
			  			  }
			  ```
		- shadowing  
			- Rust 允许声明相同的变量名，在后面声明的变量会遮蔽掉前面声明的，如下所示：  
			-  
			  ``` rust
			  			  fn main() {
			  			      let x = 5;
			  			      // 在main函数的作用域内对之前的x进行遮蔽
			  			      let x = x + 1;
			  			  
			  			      {
			  			          // 在当前的花括号作用域内，对之前的x进行遮蔽
			  			          let x = x * 2;
			  			          println!("The value of x in the inner scope is: {}", x);
			  			      }
			  			  
			  			      println!("The value of x is: {}", x);
			  			  }
			  ```
			- let 声明 和 mut 变量修改是本质不同的  
				-  
				  ``` rust
				  				  // 字符串类型
				  				  let spaces = "   ";
				  				  // usize数值类型
				  				  let spaces = spaces.len();
				  				  
				  				  // 上面代码是可行的
				  				  
				  				  let mut spaces = "   ";
				  				  spaces = spaces.len();
				  				  
				  				  $ cargo run
				  				     Compiling variables v0.1.0 (file:///projects/variables)
				  				  error[E0308]: mismatched types
				  				   --> src/main.rs:3:14
				  				    |
				  				  3 |     spaces = spaces.len();
				  				    |              ^^^^^^^^^^^^ expected `&str`, found `usize`
				  				  
				  				  error: aborting due to previous error
				  				       
				  				  // 则会报错，因为 mut 变量的类型是不可以被改变的，不允许将整数类型 usize 赋值给字符串类型
				  ```