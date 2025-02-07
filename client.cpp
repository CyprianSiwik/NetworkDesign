#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define CLIENT_PORT 9090
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in clientAddr, serverAddr;
    char buffer[BUFFER_SIZE] = "HELLO";
    int serverAddrSize = sizeof(serverAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // Bind client socket to a different port (9090)
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(CLIENT_PORT);
    clientAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        std::cerr << "Client bind failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Server address setup
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Send message
    sendto(clientSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, serverAddrSize);
    std::cout << "Sent: " << buffer << std::endl;

    // Receive response
    int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrSize);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed\n";
    }
    else {
        buffer[bytesReceived] = '\0';
        std::cout << "Received Echo: " << buffer << std::endl;
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
