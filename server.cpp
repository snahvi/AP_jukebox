// C++ program to show the example of server application in
// socket programming for Windows with bidirectional messaging
#include <cstring>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

volatile bool shouldExit = false;
SOCKET clientSocket;

DWORD WINAPI receiveMessages(LPVOID lpParam) {
    char buffer[1024];
    while (!shouldExit) {
        memset(buffer, 0, sizeof(buffer));
        int recvResult = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (recvResult == SOCKET_ERROR) {
            if (!shouldExit) {
                cerr << "Receive failed: " << WSAGetLastError() << endl;
            }
            break;
        } else if (recvResult == 0) {
            cout << "Client disconnected." << endl;
            shouldExit = true;
            break;
        } else {
            buffer[recvResult] = '\0';
            cout << "Client: " << buffer << endl;
        }
    }
    return 0;
}

DWORD WINAPI sendMessages(LPVOID lpParam) {
    string message;
    while (!shouldExit) {
        getline(cin, message);
        
        if (message == "quit" || shouldExit) {
            shouldExit = true;
            break;
        }
        
        int sendResult = send(clientSocket, message.c_str(), message.length(), 0);
        if (sendResult == SOCKET_ERROR) {
            cerr << "Send failed: " << WSAGetLastError() << endl;
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
        cerr << "WSAStartup failed: " << result << endl;
        return 1;
    }

    // creating socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    if (bind(serverSocket, (struct sockaddr*)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // listening to the assigned socket
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server listening on port 8080..." << endl;

    // accepting connection request
    clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Accept failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Client connected! Type messages to send (type 'quit' to exit):" << endl;

    // Create threads for sending and receiving using Windows API
    HANDLE receiveThread = CreateThread(NULL, 0, receiveMessages, NULL, 0, NULL);
    HANDLE sendThread = CreateThread(NULL, 0, sendMessages, NULL, 0, NULL);

    // Wait for threads to complete
    WaitForSingleObject(sendThread, INFINITE);
    shouldExit = true;
    
    // Close sockets to unblock receive thread
    closesocket(clientSocket);
    WaitForSingleObject(receiveThread, INFINITE);
    
    // Clean up thread handles
    CloseHandle(receiveThread);
    CloseHandle(sendThread);
    
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}