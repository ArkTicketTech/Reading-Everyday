## 事件循环
```
while (1) {
  events = getEvents();
  for (e in events)
    processEvent(e);
}
```

## select或者poll
```
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  // open and set up a bunch of sockets (not shown)

  // main loop
  while (1) {
    // initialize the fd_set to all zero
    fd_set readFDs;
    FD_ZERO(&readFDs);

    // now set the bits for the descriptors
    // this server is interested in
    // (for simplicity, all of them from min to max)
    int fd;
    for (fd = minFD; fd < maxFD; fd++)
      FD_SET(fd, &readFDs);

    // do the select
    int rc = select(maxFD + 1, &readFDs, NULL, NULL, NULL);

    // check which actually have data using FD_ISSET()
    for (fd = minFD; fd < maxFD; fd++) {
      if (FD_ISSET(fd, &readFDs)) {
        processFD(fd);
      }
    }
  }
}

```
- FD_SET(fd, &readFDs)
- select(maxFD + 1, &readFDs, NULL, NULL, NULL)
- FD_ISSET(fd, &readFDs)

- 这种基于事件的处理是单线程所以不需要用锁

## 异步IO
```
struct aiocb {
  int aio_fildes;              /* File descriptor */
  off_t aio_offset;            /* File offset */
  volatile void *aio_buf;      /* Location of buffer */
  size_t aio_nbytes;           /* Length of transfer */
};

```

- mac上接口结构体
- aio_error
- 基于事件的状态管理

- "continuation"：基本上，将完成处理该事件所需的信息记录在某个数据结构中。当事件发生（即磁盘I/O完成）时，查找所需的信息并处理该事件。
解决方案是将套接字描述符（`sd`）记录在某种数据结构中（例如哈希表），并以文件描述符（`fd`）作为索引。