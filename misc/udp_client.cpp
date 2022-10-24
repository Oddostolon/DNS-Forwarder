#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define BUFLEN 4096

using namespace std;

int main()
{
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1)
    {
        cerr << "Can't create client socket" << endl;
        return -1;
    }

    // Build server address
    sockaddr_in serverAddress;
    socklen_t serverSize = sizeof(serverAddress);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);

    // While loop: accept and echo message back to client
    char buf[BUFLEN];
    string userInput;
    while (true)
    {
        // Enter input message
        cout << "Enter a message to send: ";
        getline(cin, userInput);

        // Send message to server by server address
        int sendRes = sendto(clientSocket, userInput.c_str(), userInput.size() + 1, 0, (sockaddr *)&serverAddress, serverSize);
        if (sendRes == -1)
        {
            cout << "Error: Could not send message to server by server address\r\n";
            continue;
        }

        // Wait for response from server
        memset(buf, 0, BUFLEN);
        int bytesReceived = recvfrom(clientSocket, buf, BUFLEN, 0, (sockaddr *)&serverAddress, &serverSize);
        if (bytesReceived == -1)
        {
            cout << "Error in recvfrom()\r\n";
        }
        else
        {
            // Display response
            cout << "Response from server: " << string(buf, bytesReceived) << "\r\n";
        }
    }

    // Close the socket
    close(clientSocket);

    return 0;
}