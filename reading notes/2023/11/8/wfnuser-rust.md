- #Rust语言圣经 #寻找牛刀，以便小试  
	- 创建项目 - `cargo new world_hello`  
	- 运行项目 - `cargo run`  
		- 等价于 先 `cargo build` 然后 `./target/debug/world_hello`  
		- `cargo run --release` `cargo build --release` 用于生成优化后的代码  
		- `cargo check` 用于快速检查代码能否编译通过，运行速度很快  
			- rust编译器的工作很多，因此比许多语言编译更慢  
			- 非 release 版本编译也会快的多  
	- cargo.toml & cargo.lock  
		- `Cargo.toml` 是 `cargo` 特有的**项目数据描述文件**。它存储了项目的所有元配置信息，如果 Rust 开发者希望 Rust 项目能够按照期望的方式进行构建、测试和运行，那么，必须按照合理的方式构建 `Cargo.toml`。  
		- `Cargo.lock` 文件是 `cargo` 工具根据同一项目的 `toml` 文件生成的**项目依赖详细清单**，因此我们一般不用修改它，只需要对着 `Cargo.toml` 文件撸就行了。  
		-  
		  > 什么情况下该把 `Cargo.lock` 上传到 git 仓库里？  
		  很简单，当你的项目是一个可运行的程序时，就上传 `Cargo.lock`，如果是一个依赖库项目，那么请把它添加到 `.gitignore` 中。  

		- 定义项目依赖  
			-  
			  ``` toml
			  			  [dependencies]
			  			  rand = "0.3"
			  			  hammer = { version = "0.5.0"}
			  			  color = { git = "https://github.com/bjz/color-rs" }
			  			  geometry = { path = "crates/geometry" }
			  ```
			-  
		-  