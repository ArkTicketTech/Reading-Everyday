- 2.6 模式匹配
	- match 和 if let
		- 例子
			-
			  ```rust
			  enum Direction {
			      East,
			      West,
			      North,
			      South,
			  }
			  
			  fn main() {
			      let dire = Direction::South;
			      match dire {
			          Direction::East => println!("East"),
			          Direction::North | Direction::South => {
			              println!("South or North");
			          },
			          _ => println!("West"),
			      };
			  }
			  ```
		- 模式绑定
			-
			  ```rust
			  enum Action {
			      Say(String),
			      MoveTo(i32, i32),
			      ChangeColorRGB(u16, u16, u16),
			  }
			  
			  fn main() {
			      let actions = [
			          Action::Say("Hello Rust".to_string()),
			          Action::MoveTo(1,2),
			          Action::ChangeColorRGB(255,255,0),
			      ];
			      for action in actions {
			          match action {
			              Action::Say(s) => {
			                  println!("{}", s);
			              },
			              Action::MoveTo(x, y) => {
			                  println!("point from (0, 0) move to ({}, {})", x, y);
			              },
			              Action::ChangeColorRGB(r, g, _) => {
			                  println!("change color into '(r:{}, g:{}, b:0)', 'b' has been ignored",
			                      r, g,
			                  );
			              }
			          }
			      }
			  }
			  ```
		- if let - **你只要匹配一个条件，且忽略其他条件时就用 `if let` ，否则都用 `match`**
		-
		  ```rust
		      if let Some(3) = v {
		          println!("three");
		      }
		  
		      let v = Some(3u8);
		      match v {
		          Some(3) => println!("three"),
		          _ => (),
		      }
		  ```
		-