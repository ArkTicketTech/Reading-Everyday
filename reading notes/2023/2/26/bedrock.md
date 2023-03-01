```go
package main

import (
	"log"
	"net"
	"time"
)

func main() {
	// 1. 向服务器开启一个TCP链接
	c, err := net.Dial("tcp", "localhost:8000")
	if err != nil {
		log.Fatalf("Error to open TCP connection: %s", err)
	}
	defer c.Close()

	// 2. 向服务器写一些数据
	log.Printf("Tcp session open")
	b := []byte("Hi, gopher?")
	_, err = c.Write(b)
	if err != nil {
		log.Fatalf("Error writing TCP session: %s", err)
	}

	// 3. 创建一个10s后关闭TCP连接的协程
	go func() {
		<-time.After(time.Duration(10) * time.Second)
		defer c.Close()
	}()

	// 4. 读取任何回复直到读到错误
	for {
		d := make([]byte, 100)
		_, err := c.Read(d)
		if err != nil {
			log.Fatalf("Error reading TCP session: %s", err)
		}
		log.Fatalf("Reading data from server: %s\n", string(d))
	}
}

```
