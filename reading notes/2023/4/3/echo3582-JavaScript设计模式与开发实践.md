看了代理模式，作者写的挺有意思。

A给B送花 但是A对B不了解，不知道B啥时候心情好，所以找来了B的闺蜜C监听B啥时候心情好，再给她送花。

然后又举了一个图片预加载的例子，通过代理模式可以更好的践行单一职责原则和开放封闭原则。

图片预加载代码示例：

```
      var myImage = (function(){
            var imgNode = document.createElement( 'img' );
            document.body.appendChild( imgNode );

            return {
              setSrc: function( src ){
                  imgNode.src = src;
              }
            }
        })();

        var proxyImage = (function(){
            var img = new Image;
            img.onload = function(){
              myImage.setSrc( this.src );
            }
            return {
              setSrc: function( src ){
                  myImage.setSrc( 'file:// /C:/Users/svenzeng/Desktop/loading.gif' );
                  img.src = src;
              }
            }
        })();

        proxyImage.setSrc( 'http://imgcache.qq.com/music/photo/k/000GGDys0yA0Nk.jpg' );
```

> 实际上，我们需要的只是给img节点设置src，预加载图片只是一个锦上添花的功能。如果能把这个操作放在另一个对象里面，自然是一个非常好的方法。于是代理的作用在这里就体现出来了，代理负责预加载图片，预加载的操作完成之后，把请求重新交给本体MyImage。
