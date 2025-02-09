#include <iostream> 
#include <fstream> //file stream support
#include <winsock2.h> //header for windows sockets API, lets me develop compile and run in windows with no linux

#pragma comment(lib, "ws2_32.lib") //links library to project, needed for socket programming

#define SERVER_PORT 8080 //port number on which server listens, client sends data to this port
#define CLIENT_PORT 9090 //clients local port number, client uses this port to send and receive data
#define SERVER_IP "127.0.0.1" //servers IP address, local host, client sends data to this IP
#define PACKET_SIZE 1024 // defines packet size for file transmission in bytes. controls how large each chunk of the file will be when sent using the UDP protocol


// sendMessage function - sends a message from client to the server
// SOCKET is a type defined in Winsock API, a network endpoint
// clientSocket is passed as an argument and is a variable that stores the resulting socket descriptor, allowing the function to send data through that particular socket
// sockaddr_in is a structure in Winsock, provides the address of the server to which the client will send messages
void sendMessage(SOCKET clientSocket, sockaddr_in serverAddr) {
    char buffer[PACKET_SIZE]; //buffer to store the message data to be sent

    std::cout << "Enter message to send: "; //prompt message
    std::cin.ignore();  // Ignore leftover newline from previous input
    std::cin.getline(buffer, PACKET_SIZE); //reads users input message and stores it in the buffer

    // sendto - sends the message stored in (buffer) to the server at serverAddr
    // clientSocket is the socket descriptor, the communication endpoint on the client side.
    // 0 - no special flags,  serverAddr is a sockaddr structure that holds the address information of the server, tells sendto where to send the message
    sendto(clientSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    std::cout << "Message sent: " << buffer << std::endl;

    int serverAddrSize = sizeof(serverAddr);
    int bytesReceived = recvfrom(clientSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrSize); // waits for a message from the server and stores it in buffer
    if (bytesReceived > 0) { // if byte received
        buffer[bytesReceived] = '\0'; //ensures proper null termination
        std::cout << "Echoed message: " << buffer << std::endl; //displays echoed message
    }
}

// handles sending a bmp file from client to server
void sendFile(SOCKET clientSocket, sockaddr_in serverAddr) { //sends file - client socket to server socket
    char filename[100]; // buffer to store the file name input
    std::cout << "Enter BMP filename to send: "; // Use full path using 2 {\\} characters per slash
    std::cin.ignore(); 
    std::cin.getline(filename, 100); //reads file name entered by user

    std::ifstream file(filename, std::ios::binary); //opens specified file for reading
    if (!file) { //error if file could not be opened
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    char buffer[PACKET_SIZE];
    int bytesRead;

    while (!file.eof()) { // while not end of file for user file, reads until finished
        file.read(buffer, PACKET_SIZE); // reads (PACKET_SIZE) bytes from the file into the buffer
        bytesRead = file.gcount();
        sendto(clientSocket, buffer, bytesRead, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)); // sends the data in buffer - the size of one packet - to the server address
        Sleep(4); // small delay
    }

    std::cout << "File transfer complete.\n";
    file.close(); // closes file stream
}

int main() {
    WSADATA wsaData; // structure for windows sockets
    SOCKET clientSocket; // define the socket used
    sockaddr_in clientAddr, serverAddr; // define socket addresses

    WSAStartup(MAKEWORD(2, 2), &wsaData); // initialize windows sockets library

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0); // create a UDP socket
    if (clientSocket == INVALID_SOCKET) { // if socket creation fails, error is displayed
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    clientAddr.sin_family = AF_INET; // specifies address family - IPv4
    clientAddr.sin_port = htons(CLIENT_PORT); // specifies the client port
    clientAddr.sin_addr.s_addr = INADDR_ANY; // binds socket to any available network interface on the local machine

    bind(clientSocket, (sockaddr*)&clientAddr, sizeof(clientAddr)); // binds the socket to local address and port, allowing client to listen for incoming messages on that port

    serverAddr.sin_family = AF_INET; // address family
    serverAddr.sin_port = htons(SERVER_PORT); // specifies the server port
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); // convert the IP address string into a binary format

    int choice; // initialize choice int to be used for options
    std::cout << "Choose operation:\n";
    std::cout << "1 - Send Message (Phase 1a)\n";
    std::cout << "2 - Send BMP File (Phase 1b)\n";
    std::cout << "Enter choice: ";
    std::cin >> choice; // reads in int value and assigns to the elseif tree below

    if (choice == 1) { // if int choice == 1 send message and prompt for the message
        sendMessage(clientSocket, serverAddr); // calls sendMessage function
    }
    else if (choice == 2) { // if int choice == 2 send file and prompt for file path
        sendFile(clientSocket, serverAddr); // calls sendFile function
    }
    else { // error if choice is not valid
        std::cout << "Invalid choice. Exiting...\n";
    }

    closesocket(clientSocket); // closes socket after use
    WSACleanup(); // cleans up windows sockets library before the program ends.
    return 0; // exit file
}
