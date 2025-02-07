//main cpp file
#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    int clientAddrSize = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // Bind socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "UDP Server is running on port " << SERVER_PORT << "...\n";

    // Receive message
    int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed\n";
    }
    else {
        buffer[bytesReceived] = '\0'; // Null-terminate the received message
        std::cout << "Received: " << buffer << std::endl;

        // Echo message back to client
        sendto(serverSocket, buffer, bytesReceived, 0, (sockaddr*)&clientAddr, clientAddrSize);
    }

    // Cleanup
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
