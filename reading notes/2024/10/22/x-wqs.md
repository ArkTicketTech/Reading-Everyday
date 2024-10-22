litecoin

这个puzzle好的地方，对矿工来说挖矿是memory hard。坏的地方，对轻节点也是memory hard。设计puzzle的原则是：difficult to solve, but easy to verify。求解puzzle很难，验证puzzle很容易。但是验证和求解的区域几乎一样大。验证也要保存数组，否则计算复杂度很高。对于litecoin而言，不是问题，它没有轻节点验证的问题。但对我们这个场景而言，是不行的。这样造成的结果是，litecoin真正使用的时候，内存区域不敢设置太大。对于计算机1G不大，手机就太大了。实际使用的时候，数组只有128K，非常小。连1M都不到，就是为了照顾轻节点。

效果如何？
发行的时候，目标不仅是asic resistance 还是 gpu resistance。大家都用普通的cpu来挖矿。后来就出现了gpu 挖矿，后面asic 芯片挖矿。实践证明，litecoin要求的128k，不足以对asic芯片的生产和设计带来实质性的障碍。设计目标没有达到。但早期宣传的设计目标，对于解决莱特币的冷启动的问题很有帮助。
