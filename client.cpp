// C++ program to illustrate the client application in the
// socket programming for Windows with bidirectional messaging
#include <cstring>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

volatile bool shouldExit = false;
SOCKET clientSocket;

DWORD WINAPI receiveMessages(LPVOID lpParam) {
    char buffer[1024];
    while (!shouldExit) {
        memset(buffer, 0, sizeof(buffer));
        int recvResult = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (recvResult == SOCKET_ERROR) {
            if (!shouldExit) {
                std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            }
            break;
        } else if (recvResult == 0) {
            std::cout << "Server disconnected." << std::endl;
            shouldExit = true;
            break;
        } else {
            buffer[recvResult] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        }
    }
    return 0;
}

DWORD WINAPI sendMessages(LPVOID lpParam) {
    std::string message;
    while (!shouldExit) {
        std::getline(std::cin, message);
        
        if (message == "quit" || shouldExit) {
            shouldExit = true;
            break;
        }
        
        int sendResult = send(clientSocket, message.c_str(), message.length(), 0);
        if (sendResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            shouldExit = true;
            break;
        }
    }
    return 0;
}

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // creating socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // sending connection request
    if (connect(clientSocket, (struct sockaddr*)&serverAddress,
                sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server! Type messages to send (type 'quit' to exit):" << std::endl;

    // Create threads for sending and receiving using Windows API
    HANDLE receiveThread = CreateThread(NULL, 0, receiveMessages, NULL, 0, NULL);
    HANDLE sendThread = CreateThread(NULL, 0, sendMessages, NULL, 0, NULL);

    // Wait for threads to complete
    WaitForSingleObject(sendThread, INFINITE);
    shouldExit = true;
    
    // Close socket to unblock receive thread
    closesocket(clientSocket);
    WaitForSingleObject(receiveThread, INFINITE);
    
    // Clean up thread handles
    CloseHandle(receiveThread);
    CloseHandle(sendThread);

    WSACleanup();
    return 0;
}