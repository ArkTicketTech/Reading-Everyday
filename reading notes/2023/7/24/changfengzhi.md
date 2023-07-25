   `let xhr = new XMLHttpRequest();`

​        `xhr.open("GET", "/giteaServerApi/user/oauth2/SW_Community",true);`

​        `xhr.onreadystatechange = ()=> {

​           window.location.href = xhr.getResponseHeader("Location");`

​          `let hha=xhr.getResponseHeader("Location");`

​        `};`

​        `xhr.send(null);`

#### readyState属性 返回当前请求的状态

- 0 (未初始化) 对象已建立，但是尚未初始化（尚未调用open方法）
- 1 (初始化) 对象已建立，尚未调用send方法
- 2 (发送数据) send方法已调用，但是当前的状态及http头未知
- 3 (数据传送中) 已接收部分数据，因为响应及http头不全，这时通过responseBody和responseText获取部分数据会出现错误
- 4 (完成) 数据接收完毕,此时可以通过通过responseBody和responseText获取完整的回应数据