# win-socket学习

## 1.一些需要说明的基础：

端口:

端口也是一个整形数 unsigned short ，一个16位整形数，有效端口的取值范围是：$0 到 65535(0 到 2^{16-1})$


主机字节序 (小端):

数据的低位字节存储到内存的低地址位, 数据的高位字节存储到内存的高地址位
我们使用的PC机，数据的存储默认使用的是小端

网络字节序 (大端):

据的低位字节存储到内存的高地址位, 数据的高位字节存储到内存的低地址位
套接字通信过程中操作的数据都是大端存储的，包括：接收/发送的数据、IP地址、端口。

## 2. Server流程说明

### 2.1.创建套接字

```
// 创建一个套接字
int socket(int domain, int type, int protocol);
```

#### 参数:
* domain: 使用的地址族协议
  * AF_INET: 使用IPv4格式的ip地址
  * AF_INET6: 使用IPv6格式的ip地址
* type:
  * SOCK_STREAM: 使用流式的传输协议
  * SOCK_DGRAM: 使用报式(报文)的传输协议
* protocol: 一般写0即可, 使用默认的协议
  * SOCK_STREAM: 流式传输默认使用的是tcp
  * SOCK_DGRAM: 报式传输默认使用的udp
* 返回值:
  * 成功: 可用于套接字通信的文件描述符
  * 失败: -1

### 2.2 绑定socket
创建套接字
```
# 创建套接字
struct sockaddr_in
{
    sa_family_t sin_family;		/* 地址族协议: AF_INET */
    in_port_t sin_port;         /* 端口, 2字节-> 大端  */
    struct in_addr sin_addr;    /* IP地址, 4字节 -> 大端  */
    /* 填充 8字节 */
    unsigned char sin_zero[sizeof (struct sockaddr) - sizeof(sin_family) -
               sizeof (in_port_t) - sizeof (struct in_addr)];
}; 

```

将文件描述符和本地的IP与端口进行绑定   

``` 
// 将文件描述符和本地的IP与端口进行绑定   
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

#### 参数:

* sockfd: 监听的文件描述符, 通过socket()调用得到的返回值
* addr: 传入参数, 要绑定的IP和端口信息需要初始化到这个结构体中，IP和端口要转换为网络字节序
* addrlen: 参数addr指向的内存大小, sizeof(struct sockaddr)
* 返回值：成功返回0，失败返回-1

### 2.2 设置监听

给监听的套接字设置监听

```
// 给监听的套接字设置监听
int listen(int sockfd, int backlog);
```

* 参数:
  * sockfd: 文件描述符, 可以通过调用socket()得到，在监听之前必须要绑定 bind()
  * backlog: 同时能处理的最大连接要求，最大值为128
  * 返回值：函数调用成功返回0，调用失败返回 -1

### 2.3 阻塞

在这个代码中，`accept` 函数会进入阻塞状态，等待客户端的连接请求。所谓“阻塞”，就是程序会在这里停住，直到有客户端尝试连接到这个服务器。

```
// 阻塞等待并接受客户端连接 -------------
struct sockaddr_in cliaddr;  // 用于存储客户端的地址信息
int clilen = sizeof(cliaddr);  // 客户端地址结构体的大小
// accept 函数用于接受客户端的连接请求，返回一个新的套接字（用于与客户端通信）
int cfd = accept(fd, (struct sockaddr*)&cliaddr, &clilen);
if(cfd == -1) {  // 如果接受连接失败，输出错误信息
	perror("accept");
    return -1;
}
```

### 2.4 接收内容

```
char buff[1024];  // 定义一个缓冲区用于接收数据
ssize_t recv(int sockfd, void *buf, size_t size, int flags);
```

* 参数:
  * sockfd: 用于通信的文件描述符, accept() 函数的返回值
  * buf: 指向一块有效内存, 用于存储接收是数据
  * size: 参数buf指向的内存的容量
  * flags: 特殊的属性, 一般不使用, 指定为 0
  * 返回值:
  	* 大于0：实际接收的字节数
  	* 等于0：对方断开了连接
  	* -1：接收数据失败了

### 2.4 结束
```
// 关闭套接字 -------------
closesocket(cfd);  // 关闭客户端套接字
closesocket(fd);   // 关闭服务器套接字

// 清理 Winsock -------------
WSACleanup();  // 释放 Winsock 库
```

## 3. Client流程
#### 3.1 填入服务器ip，并且转换一下ip
在定义套接字以前，都是一样的，使用inet_pton 的原因是将人类可读的字符串形式 IP 地址（如 "172.26.168.143"）转换成二进制格式。
```
#include <arpa/inet.h>
// 将大端的整形数, 转换为小端的点分十进制的IP地址
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```
* 参数:
  * af: 地址族协议
    * AF_INET: ipv4格式的ip地址
    * AF_INET6: ipv6格式的ip地址
  * src: 传入参数, 这个指针指向的内存中存储了大端的整形IP地址
  * dst: 传出参数, 存储转换得到的小端的点分十进制的IP地址
  * size: 修饰dst参数的, 标记dst指向的内存中最多可以存储多少个字节
  * 返回值:
    * 成功: 指针指向第三个参数对应的内存地址, 通过返回值也可以直接取出转换得到的IP字符串
    * 失败: NULL
#### 3.2 连接服务器
```
// 成功连接服务器之后, 客户端会自动随机绑定一个端口
// 服务器端调用accept()的函数, 第二个参数存储的就是客户端的IP和端口信息
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
* 参数:
* sockfd: 通信的文件描述符, 通过调用socket()函数就得到了
* addr: 存储了要连接的服务器端的地址信息: iP 和 端口，这个IP和端口也需要转换为大端然后再赋值
* addrlen: addr指针指向的内存的大小 sizeof(struct sockaddr)
* 返回值：
  * 连接成功返回0
  * 连接失败返回-1

#### 3.3 发送和接收消息
```
// 接收数据
ssize_t send(int fd, const void *buf, size_t len, int flags);
ssize_t recv(int fd, void *buf, size_t size, int flags);
```
* 参数:
  * fd: 通信的文件描述符, accept() 函数的返回值
  * buf: 传入参数, 要发送的字符串
  * len: 要发送的字符串的长度
  * flags: 特殊的属性, 一般不使用, 指定为 0
  * 返回值：
    * 大于0：实际发送的字节数，和参数len是相等的
    * -1：发送数据失败了
