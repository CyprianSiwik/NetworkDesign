//main cpp file
#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024


int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "UDP Server listening on port " << SERVER_PORT << "...\n";

    while (true) {
        int clientLen = sizeof(clientAddr);
        int recvLen = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &clientLen);

        if (recvLen == SOCKET_ERROR) {
            std::cerr << "Receive failed\n";
            continue;
        }

        buffer[recvLen] = '\0';  // Null-terminate the message
        std::cout << "Received: " << buffer << std::endl;

        // Echo message back to client
        sendto(serverSocket, buffer, recvLen, 0, (struct sockaddr*)&clientAddr, clientLen);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
