控制代码被执行的几率

-   在一个`select`流程控制中使用重复的`case`操作来增加对应分支中的代码的执行几率(...6)



从动态数量的分支中选择

-   可以使用`reflect`标准库包中提供的功能在运行时刻来构建动态分支数量的select控制流
    程
-   一个`select`控制流程中的分支越多，此`select`控制流程的执行效率就越低（常常只使用不多于三个分支的`select`控制流程的原因）
-   `reflect`标准库包中也提供了模拟尝试发送和尝试接收代码块的`TrySend`和`TryRecv`函数



数据流操纵

-   一个模块中的工作协程从一些其它模块接收数据做为输入，并向另一些模块发送输出数据(一个模块可能同时兼任数据消费者和数据产生者的角色)



数据生成/搜集/加载(略)



数据聚合

-   一个数据聚合模块的工作协程将多个数据流合为一个数据流

    ```
    func Aggregator(inputs ...<-chan uint64) <-chan uint64 {
    	output := make(chan uint64)
    	var wg sync.WaitGroup
    	for _, in := range inputs {
    		wg.Add(1)
    		go func(int <-chan uint64) {
    			defer wg.Done()
    			// 如果通道in被关闭，此循环将最终结束。
    			for x := range in {
    				output <- x
    			}
    		}(in)
    	}
    	go func() {
    		wg.Wait()
    		close(output)
    	}()
    	return output
    }
    ```

    ```
    // 假设数据流的数量为2。
    ...
    output := make(chan uint64)
    go func() {
    	inA, inB := inputs[0], inputs[1]
    	for {
    		select {
    		case v := <- inA: output <- v
    		case v := <- inB: output <- v
    		}
    	}
    }
    ...
    ```



数据分流(略)



数据合成

-   数据合成将多个数据流中读取的数据合成一个

    ```
    func Composor(inA, inB <-chan uint64) <-chan uint64 {
    	output := make(chan uint64)
    	go func() {
    		for {
    			a1, b, a2 := <-inA, <-inB, <-inA
    			output <- a1 ^ b & a2
    		}
    	}()
    	return output
    }
    ```



数据复制/增殖

-   数据复制（增殖）可以看作是特殊的数据分解。一份输入数据将被复制多份并输出给多个数据流

    ```
    func Duplicator(in <-chan uint64) (<-chan uint64, <-chan uint64) {
    	outA, outB := make(chan uint64), make(chan uint64)
    	go func() {
    		for x := range in {
    			outA <- x
    			outB <- x
    		}
    	}()
    	return outA, outB
    }
    ```

    

数据计算/分析（略）

数据验证/过滤（略）

数据服务/存盘（略）都差不多



Pp. 385-398