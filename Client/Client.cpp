#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")  // 连接到 Winsock 库

int main() {
    // 初始化 Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // 创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // 服务器信息设置
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET, "172.26.168.143", &saddr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == SOCKET_ERROR) {
        std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(fd);
        WSACleanup();
        return 1;
    } else {
        std::cout << "Connected to the server successfully!" << std::endl;
    }

    // 与服务器通信
    while (true) {
        char sendBuffer[1024];
        char recvBuffer[1024];

        std::cout << "请输入要发送的信息: ";
        std::cin.getline(sendBuffer, sizeof(sendBuffer));

        int sendResult = send(fd, sendBuffer, strlen(sendBuffer) + 1, 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "发送失败." << std::endl;
            break;
        }

        int recvResult = recv(fd, recvBuffer, sizeof(recvBuffer), 0);
        if (recvResult > 0) {
            recvBuffer[recvResult] = '\0';
            std::cout << "Server says: " << recvBuffer << std::endl;
        } else if (recvResult == 0) {
            std::cout << "Server closed the connection." << std::endl;
            break;
        } else {
            std::cerr << "Recv failed." << std::endl;
            break;
        }
    }


    // 清理工作
    closesocket(fd);
    WSACleanup();

    return 0;
}
