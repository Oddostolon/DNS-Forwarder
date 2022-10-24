#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <memory>

#define BUFLEN 1024

class dns_forward_server
{
    public: 
        int init(std::string upstream_address, int upstream_port);
        int run();
        ~dns_forward_server()
        {
            close(network_socket_client);
            close(network_socket_upstream);
        }

    private: 
        sockaddr_in server_address;
        sockaddr_in upstream_server;

        int network_socket_client;
        int network_socket_upstream; 

        char buffer[BUFLEN];

        int send_message_from_buffer(int message_size, int sockaddr_size);
        int receive_message_into_buffer(sockaddr_in sender_address, socklen_t sender_address_size);
};