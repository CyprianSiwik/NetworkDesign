#include <iostream>
#include <fstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8080
#define PACKET_SIZE 1024

void receiveFile(SOCKET serverSocket, sockaddr_in clientAddr) {
    std::ofstream file("SentImage.bmp", std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not create file\n";
        return;
    }

    char buffer[PACKET_SIZE];
    int clientAddrSize = sizeof(clientAddr);
    int bytesReceived;

    std::cout << "Receiving file...\n";

    while ((bytesReceived = recvfrom(serverSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize)) > 0) {
        file.write(buffer, bytesReceived);
    }

    std::cout << "File received successfully.\n";
    file.close();
}

void receiveMessage(SOCKET serverSocket, sockaddr_in clientAddr) {
    char buffer[PACKET_SIZE];
    int clientAddrSize = sizeof(clientAddr);

    int bytesReceived = recvfrom(serverSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Received message: " << buffer << std::endl;

        // Echo back the message
        sendto(serverSocket, buffer, bytesReceived, 0, (sockaddr*)&clientAddr, clientAddrSize);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr, clientAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    std::cout << "Server is running. Waiting for data...\n";

    while (true) {
        char buffer[PACKET_SIZE];
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        int bytesReceived = recvfrom(serverSocket, buffer, PACKET_SIZE, MSG_PEEK, (sockaddr*)&clientAddr, &clientAddrSize);

        if (bytesReceived > 0) {
            if (bytesReceived < PACKET_SIZE) {
                receiveMessage(serverSocket, clientAddr);
            }
            else {
                receiveFile(serverSocket, clientAddr);
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
