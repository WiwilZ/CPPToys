## 真随机数实现

- linux

  通过读取 `"/dev/urandom"`（非阻塞方式）或`"/dev/random"`（阻塞方式）熵池获取随机字节

- windows

  通过`CryptGenRandom`API获取随机字节

