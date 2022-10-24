#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9999
#define BUFLEN 4096
 
using namespace std;
 
int main()
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1)
    {
        cerr << "Error: Can't create a socket" << endl;
        return -1;
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);
 
    bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
 
    // While loop: accept and echo message back to client
    char buf[BUFLEN];
 
    while (true)
    {
        // Wait for client to send data
        memset(buf, 0, BUFLEN);
        sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);
        int bytesReceived = recvfrom(serverSocket, buf, BUFLEN, 0, (sockaddr*)&clientAddress, &clientSize);
        if (bytesReceived == -1)
        {
            cerr << "Error in recvfrom()" << endl;
            break;
        }
 
        cout << string(buf, 0, bytesReceived) << endl;
 
        // Echo message back to client
        sendto(serverSocket, buf, bytesReceived + 1, 0, (sockaddr*)&clientAddress, clientSize);
    }
 
    // Close the socket
    close(serverSocket);
 
    return 0;
}