### 鸭子类型
> 鸭子类型的通俗说法是：“如果它走起路来像鸭子，叫起来也是鸭子，那么它就是鸭子。”

> 鸭子类型指导我们只关注对象的行为，而不关注对象本身，也就是关注HAS-A，而不是IS-A。

```
        var duck = {
            duckSinging: function(){
              console.log( ’嘎嘎嘎’ );
        };
            }

        var chicken = {
            duckSinging: function(){
              console.log( ’嘎嘎嘎’ );
            }
        };

        var choir = [];    //合唱团

        var joinChoir = function( animal ){
            if ( animal && typeof animal.duckSinging === 'function' ){
                choir.push( animal );
                console.log( ’恭喜加入合唱团’ );
                console.log( ’合唱团已有成员数量：' + choir.length );
            }
        };

        joinChoir( duck );    //恭喜加入合唱团
        joinChoir( chicken );    //恭喜加入合唱团
```

> 在动态类型语言的面向对象设计中，鸭子类型的概念至关重要。利用鸭子类型的思想，我们不必借助超类型的帮助，就能轻松地在动态类型语言中实现一个原则：“面向接口编程，而不是面向实现编程”。

至于静态类型语言为什么一定要借助超类才可以，我们留待下回分解。
