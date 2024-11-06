#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")  // 连接到 Winsock 库

int main() {
    // 初始化 Winsock -------------
    WSADATA wsaData;
    // 调用 WSAStartup 初始化 Winsock，MAKEWORD(2, 2) 请求使用 Winsock 2.2 版本
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;  // 如果初始化失败，打印错误
        return 1;  // 返回错误代码
    }

    // 创建套接字 -------------
    // socket 函数用于创建一个新的套接字，返回一个套接字描述符
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 使用 IPv4 协议和 TCP 套接字类型
    if (sockfd == INVALID_SOCKET) {  // 如果套接字创建失败
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();  // 清理 Winsock 库
        return 1;  // 返回错误代码
    }

    // 服务器信息设置 -------------
    struct sockaddr_in serverAddr;  // 用于存储服务器的地址信息
    serverAddr.sin_family = AF_INET;  // 设置地址簇为 IPv4 协议
    serverAddr.sin_port = htons(9999);  // 设置端口号，使用 htons() 将端口转换为网络字节顺序
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 设置服务器 IP 地址为本地地址（127.0.0.1）

    // 连接服务器 -------------
    // 调用 connect 函数连接到指定的服务器
    iResult = connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {  // 如果连接失败
        std::cerr << "Connect failed." << std::endl;
        closesocket(sockfd);  // 关闭套接字
        WSACleanup();  // 清理 Winsock 库
        return 1;  // 返回错误代码
    }
    std::cout << "Connected to server!" << std::endl;  // 如果连接成功，打印消息

    // 与服务器通信 -------------
    char sendBuffer[1024];  // 定义缓冲区用于存储发送给服务器的数据
    char recvBuffer[1024];  // 定义缓冲区用于接收从服务器返回的数据
    while (true) {
        std::cout << "Enter message to send to server: ";
        std::cin.getline(sendBuffer, sizeof(sendBuffer));  // 从用户输入中获取一行消息

        // 发送消息到服务器 -------------
        // send 函数将缓冲区的数据发送到服务器
        int sendResult = send(sockfd, sendBuffer, strlen(sendBuffer), 0);
        if (sendResult == SOCKET_ERROR) {  // 如果发送失败
            std::cerr << "Send failed." << std::endl;
            break;  // 退出循环
        }

        // 接收来自服务器的回复 -------------
        // recv 函数从服务器接收数据
        int recvResult = recv(sockfd, recvBuffer, sizeof(recvBuffer), 0);
        if (recvResult > 0) {  // 如果接收到数据
            recvBuffer[recvResult] = '\0';  // 确保接收到的消息以 NULL 字符结束（转化为字符串）
            std::cout << "Server says: " << recvBuffer << std::endl;  // 打印服务器返回的消息
        } else if (recvResult == 0) {  // 如果返回 0，表示服务器已关闭连接
            std::cout << "Server closed the connection." << std::endl;
            break;  // 退出循环，停止通信
        } else {  // 如果发生错误
            std::cerr << "Recv failed." << std::endl;
            break;  // 退出循环
        }
    }

    // 清理工作 -------------
    closesocket(sockfd);  // 关闭套接字，释放资源
    WSACleanup();  // 清理 Winsock 库

    return 0;  // 正常退出
}
