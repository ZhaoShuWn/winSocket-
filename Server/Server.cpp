#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
// 引入 Winsock 2.0 相关的库，ws2_32.lib 是 Windows 下使用 Winsock 进行网络编程时所需要链接的库。

int main() {
    // 初始化 Winsock ------------- start （这段是固定写法，不需要修改）
    WSADATA wsaData;
    // 调用 WSAStartup 函数初始化 Winsock 库，参数 MAKEWORD(2, 2) 表示请求使用 Winsock 2.2 版本
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult); // 如果初始化失败，打印错误信息
        return 1;
    }
    // 初始化 Winsock ------------- end
    

    // 创建套接字 -------------
    // socket 函数用于创建一个新的套接字，返回一个套接字描述符
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    // socket 参数说明：
    // AF_INET：使用 IPv4 协议；若使用 IPv6，可以使用 AF_INET6
    // SOCK_STREAM：创建一个面向连接的套接字（TCP）
    // 0：使用默认的协议，通常是 TCP（也可以指定 IPPROTO_TCP）
    if (fd == INVALID_SOCKET) { // 检查套接字创建是否成功
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    // AF_INET 和 AF_INET6 解释：
    // AF_INET：IPv4协议，即使用 32 位地址（如：192.168.1.1）
    // AF_INET6：IPv6协议，即使用 128 位地址（如：fe80::d4a8:6435:d2d8:8a2e）
    //   - IPv6 能支持更多的IP地址空间，尤其适用于大规模设备的网络通信。
    //   - 当选择 AF_INET6 时，套接字处理 IPv6 地址，不同的是其地址格式和相关函数（如 inet_pton，inet_ntop）有所不同。

    // 绑定套接字 -------------
    // bind 函数将套接字与本地地址进行绑定，允许该套接字监听来自指定端口的请求
    struct sockaddr_in saddr;  // sockaddr_in 结构体用于存储 IPv4 地址信息
    saddr.sin_family = AF_INET;  // 地址簇，指定使用 IPv4 协议族
    saddr.sin_port = htons(9999);  // 将端口号转换为网络字节顺序（注意，网络字节序是大端序）
    saddr.sin_addr.s_addr = INADDR_ANY;  // 绑定到所有本机的 IP 地址

    // bind 函数将本地 IP 地址和端口号绑定到套接字
    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret == -1) {  // 如果绑定失败，输出错误信息
        perror("bind");
        return -1;
    }

    // listen 函数用于将套接字转换为监听状态，准备接收客户端的连接请求
    // 第二个参数是等待队列的最大长度，表示操作系统为此监听套接字分配的缓冲队列大小
    ret = listen(fd, 128);  // 128 是最大等待队列长度
    if(ret == -1) {  // 如果监听失败，输出错误信息
        perror("listen");
        return -1;
    }

    // 阻塞等待并接受客户端连接 -------------
    struct sockaddr_in cliaddr;  // 用于存储客户端的地址信息
    int clilen = sizeof(cliaddr);  // 客户端地址结构体的大小
    // accept 函数用于接受客户端的连接请求，返回一个新的套接字（用于与客户端通信）
    int cfd = accept(fd, (struct sockaddr*)&cliaddr, &clilen);
    if(cfd == -1) {  // 如果接受连接失败，输出错误信息
        perror("accept");
        return -1;
    }

    // 输出客户端的 IP 地址和端口
    char ip[24] = {0};  // 用于存储客户端的 IP 地址（转换为字符串）
    printf("客户端的IP地址: %s, 端口: %d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),  // inet_ntop 将二进制的 IP 地址转换为点分十进制字符串
           ntohs(cliaddr.sin_port));  // ntohs 将网络字节顺序的端口号转换为主机字节顺序

    // 处理客户端通信 -------------
    while(true) {
        // 接收客户端发送的数据
        char buff[1024];  // 定义一个缓冲区用于接收数据
        int len = recv(cfd, buff, sizeof(buff), 0);  // recv 用于接收数据，返回接收到的字节数
        if(len > 0) {  // 如果成功接收到数据
            printf("client say: %s\n", buff);  // 打印客户端发来的消息
            send(cfd, buff, len, 0);  // 将接收到的数据原封不动地发送回客户端
        } else if(len == 0) {  // 如果 len 为 0，表示客户端已关闭连接
            printf("客户端断开连接\n");
            break;  // 退出循环，关闭连接
        } else {  // 如果 recv 返回负值，表示发生了错误
            perror("recv");
            break;  // 退出循环，关闭连接
        }
    }

    // 关闭套接字 -------------
    closesocket(cfd);  // 关闭客户端套接字
    closesocket(fd);   // 关闭服务器套接字

    // 清理 Winsock -------------
    WSACleanup();  // 释放 Winsock 库

    return 0;
}
