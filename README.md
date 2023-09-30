To compile and run the server and client applications, you can follow these steps:

1. Save the server code in a file called server.cpp and the client code in a file called client.cpp.

2. Compile the server code by running the following command in the terminal:
g++ server.cpp -o server -lWs2_32
This will create an executable file called server.

3. Run the server by executing the following command:
.\server.exe
This will start the server and listen for incoming connections on port 12345.

4. Open a new terminal window and compile the client code by running the following command:
g++ client.cpp -o client -lWs2_32 
This will create an executable file called client.

5. Run the client by executing the following command:
.\client.exe 
This will connect to the server and start sending messages to it.

You can run multiple instances of the client in different terminal windows to simulate multiple clients connecting to the server. When a client sends a message to the server, the server will echo it back to the client.

Run the server application first, followed by the client application.

Note:
1. This code uses the Windows Sockets API (Winsock) and is designed to be compiled in g++ and run on a Windows system. You will need to link against the ws2_32.lib library when compiling the code.

2. If you're using a different compiler or library, you'll need to modify the command accordingly.

3. This code assumes that the server is listening on port 12345 and it has SERVER_IP "127.0.0.1". You will need to change the SERVER_PORT and SERVER_IP constant to match your specific setup.

IP address used in the client code (`127.0.0.1`) assumes that the server is running on the same machine as the client. If the server is running on a different machine, you will need to use the server's actual IP address instead.


