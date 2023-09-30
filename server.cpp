#include <iostream>
#include <winsock2.h>
#include <windows.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 12345

const int BUFFER_SIZE = 2056;

// Handle the connection by receiving and echoing messages

void handleClient(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        // Echo the message back to the client
        int bytes_sent = send(clientSocket, buffer, bytesReceived, 0);
        if (bytes_sent < 0)
        {
            cerr << "Error sending data to client." << endl;
            break;
        }
        cout << "Sent: " << buffer << endl;
        memset(buffer, 0, BUFFER_SIZE);
    }
    cout << "Client Disconnected" << endl;
    closesocket(clientSocket);
}

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    // Create a socket for listening

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cerr << "socket failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to the server port

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "bind failed with error: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "listen failed with error: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port " << SERVER_PORT << '\n';

    // Wait for incoming connections and handle them

    while (true)
    {
        // Accept a new connection
        sockaddr_in clientAddress;
        int clientAddressSize = sizeof(clientAddress);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddress, &clientAddressSize);
        if (clientSocket == INVALID_SOCKET)
        {
            cerr << "Accept failed with error: " << WSAGetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ": " << ntohs(clientAddress.sin_port) << endl;

        // create a new thread for each client

        HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handleClient, (LPVOID)clientSocket, 0, NULL);
        if (threadHandle == NULL)
        {
            cerr << "CreateThread failed with error: " << GetLastError() << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // detach the thread to avoid memory leaks
        if (!CloseHandle(threadHandle))
        {
            cerr << "Error detaching thread: " << GetLastError() << endl;
        }
    }
    // cleanup
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}