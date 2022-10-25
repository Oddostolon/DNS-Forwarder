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
            close(network_socket_client_6);
            close(network_socket_upstream);
            close(network_socket_upstream_6);
        }

    private: 
        char buffer[BUFLEN];

        int init_ipv4_resources(std::string upstream_address, int upstream_port);
        int init_ipv6_resources(std::string upstream_address, int upstream_port);

        int forward_ipv4_request();
        int forward_ipv6_request();

        // IPv4 resources
        sockaddr_in server_address;
        sockaddr_in upstream_server;

        int network_socket_client;
        int network_socket_upstream; 

        // IPv6 resources
        sockaddr_in6 server_address_6;
        sockaddr_in6 upstream_server_6;

        int network_socket_client_6;
        int network_socket_upstream_6;
};