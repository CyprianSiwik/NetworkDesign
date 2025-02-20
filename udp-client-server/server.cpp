//Cyprian Siwik Network Design
//This code is for Phase 1 of the programming project
//This is the server code, it listens for data being sent and is able to receive and echo messages and receive .bmp file packets and write it to a new image titled SentImage.bmp

#include <iostream>
#include <fstream>
#include <winsock2.h>

//Link the Winsock library
#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8080 //Defining the port that the server will listen on
#define PACKET_SIZE 1024 //Defining the size of each individual packet

//Function to receive file from the client
void receiveFile(SOCKET serverSocket, sockaddr_in clientAddr) {
    std::ofstream file("SentImage.bmp", std::ios::binary); //open a file to save the received data
    if (!file) { //error flag if file cannot be created
        std::cerr << "Error: Could not create file\n";
        return;
    }

    char buffer[PACKET_SIZE]; //buffer to store received data
    int clientAddrSize = sizeof(clientAddr);
    int bytesReceived;

    std::cout << "Receiving file...\n"; //lets user know that the function has been called and the file is being received

    //receiving file in packets until there is no more data being read
    while ((bytesReceived = recvfrom(serverSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize)) > 0) {
        file.write(buffer, bytesReceived); //write received data to a file
    }

    std::cout << "File received successfully.\n";
    file.close(); //Close the file when while loop breaks and no more data is being received
}

//Function to receive a message from the client and echo it back to the client
void receiveMessage(SOCKET serverSocket, sockaddr_in clientAddr) {
    char buffer[PACKET_SIZE]; //buffer the size of one packet used to store data
    int clientAddrSize = sizeof(clientAddr);

    //receive message from the client
    int bytesReceived = recvfrom(serverSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
    if (bytesReceived > 0) { //if receiving bytes
        buffer[bytesReceived] = '\0'; //null terminates the received message
        std::cout << "Received message: " << buffer << std::endl; //lets user know the message has been received

        // Echo back the message, use sendto function to send the message back to the client address
        sendto(serverSocket, buffer, bytesReceived, 0, (sockaddr*)&clientAddr, clientAddrSize);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr, clientAddr;

    //initialize winsock
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //create the udp socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) { //error message if socket cannot be completed
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }


    // set up addresses and address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT); //converts port to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;  //listen on all available interfaces

    //bind the socket to the server address and port
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    //let user know that the server is operational and listening for data
    std::cout << "Server is running. Waiting for data...\n";

    while (true) { //stays listening for data - could add termination signal
        char buffer[PACKET_SIZE];
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        //check data to see if it is a message or file
        int bytesReceived = recvfrom(serverSocket, buffer, PACKET_SIZE, MSG_PEEK, (sockaddr*)&clientAddr, &clientAddrSize);

        if (bytesReceived > 0) { //if received data is smaller than the entire packet size, treat as a message
            if (bytesReceived < PACKET_SIZE) {
                receiveMessage(serverSocket, clientAddr);
            }
            else { //assume file transferring otherwise as its using the entirety of the packet.
                receiveFile(serverSocket, clientAddr);
            }
        }
    }

    //close the socket and clean up winsock, exit
    closesocket(serverSocket); 
    WSACleanup();
    return 0;
}
