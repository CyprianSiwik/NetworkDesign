#include <iostream>
#include <fstream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8080
#define CLIENT_PORT 9090
#define SERVER_IP "127.0.0.1"
#define PACKET_SIZE 1024

void sendMessage(SOCKET clientSocket, sockaddr_in serverAddr) {
    char buffer[PACKET_SIZE];

    std::cout << "Enter message to send: ";
    std::cin.ignore();  // Ignore leftover newline from previous input
    std::cin.getline(buffer, PACKET_SIZE);

    sendto(clientSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    std::cout << "Message sent: " << buffer << std::endl;

    int serverAddrSize = sizeof(serverAddr);
    int bytesReceived = recvfrom(clientSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrSize);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Echoed message: " << buffer << std::endl;
    }
}

void sendFile(SOCKET clientSocket, sockaddr_in serverAddr) {
    char filename[100];
    std::cout << "Enter BMP filename to send: "; // Use full path using 2 {\\} characters per \
    std::cin.ignore();
    std::cin.getline(filename, 100);

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    char buffer[PACKET_SIZE];
    int bytesRead;

    while (!file.eof()) {
        file.read(buffer, PACKET_SIZE);
        bytesRead = file.gcount();
        sendto(clientSocket, buffer, bytesRead, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        Sleep(10);
    }

    std::cout << "File transfer complete.\n";
    file.close();
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in clientAddr, serverAddr;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(CLIENT_PORT);
    clientAddr.sin_addr.s_addr = INADDR_ANY;

    bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int choice;
    std::cout << "Choose operation:\n";
    std::cout << "1 - Send Message (Phase 1a)\n";
    std::cout << "2 - Send BMP File (Phase 1b)\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;

    if (choice == 1) {
        sendMessage(clientSocket, serverAddr);
    }
    else if (choice == 2) {
        sendFile(clientSocket, serverAddr);
    }
    else {
        std::cout << "Invalid choice. Exiting...\n";
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
