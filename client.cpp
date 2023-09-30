#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <chrono>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "127.0.0.1"  // Change this to the server IP address
#define SERVER_PORT 12345      // Change this to the server port number
const int MESSAGE_SIZE = 2056; // Maximum message size

void sendMessages(SOCKET clientSocket)
{
    int message_count = 0;
    double total_round_trip_time = 0.0;
    double total_throughput = 0.0;
    char message[MESSAGE_SIZE] = {0};

    // Main loop to send messages to the server
    while (true)
    {
        // Get input from the user
        cout << "Enter message (or q to quit): ";
        string input;
        getline(cin, input);

        if (input == "q")
        {
            break;
        }

        // Send the message to the server
        strncpy(message, input.c_str(), MESSAGE_SIZE - 1);

        // Record the start time and wait for the response
        auto start_time = chrono::high_resolution_clock::now();
        int iResult = send(clientSocket, message, MESSAGE_SIZE, 0);

        if (iResult == SOCKET_ERROR)
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << '\n';
            break;
        }

        memset(message, 0, MESSAGE_SIZE);
        iResult = recv(clientSocket, message, MESSAGE_SIZE, 0);
        if (iResult == SOCKET_ERROR)
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << endl;
            break;
        }
        auto end_time = chrono::high_resolution_clock::now();
        cout << "Message received: " << message << endl;

        // Calculate and display the round trip time
        message_count++;
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        double round_trip_time = duration.count() / 1000.0; // milliseconds

        total_round_trip_time += round_trip_time;
        double average_round_trip_time = total_round_trip_time / message_count;
        cout << "Round trip time: " << round_trip_time << " ms" << endl;
        cout << "Average round trip time: " << average_round_trip_time << " ms" << endl;

        // Calculate and display the running throughput rate
        double throughput = (MESSAGE_SIZE * 8.0) / (round_trip_time / 1000.0);
        total_throughput += throughput;
        double average_throughput = total_throughput / message_count;
        cout << "Average throughput: " << average_throughput << " bits/ms" << endl;
    }
}

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        cerr << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    // Create a socket for the client
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "socket failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    iResult = connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR)
    {
        cerr << "connect failed with error: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // create a new thread for send messages
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendMessages, (LPVOID)clientSocket, 0, NULL);
    if (hThread == NULL)
    {
        cerr << "Error creating thread" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    // Clean up

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
