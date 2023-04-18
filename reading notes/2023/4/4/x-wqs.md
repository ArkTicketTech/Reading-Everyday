当信号到达集线器后，会被广播到整个网络。以太网将包发到所有的设备，根据MAC地址来判断应该接收哪些包，集线器将信号广播出去。
网线接口和后面的电路部分加在一起称为一个端口，交换机的一个端口就相当于计算机上的一块网卡。网卡本身具有MAC地址，通过核对包的接收方MAC地址，如果不是发给自己的则丢弃。交换机的端口不核对接收方MAC地址，而是直接接收所有的包并存放到缓冲区中。交换机端口的MAC模块不具有MAC地址。MAC地址和端口一一对应，通过mac地址表就能判断收到的包应该转发哪个端口。