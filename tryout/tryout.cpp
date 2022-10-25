#include <arpa/inet.h>
#include <iostream>
#include <string.h>

int main()
{
    int network_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int network_socket_6 = socket(AF_INET6, SOCK_DGRAM, 0);

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    sockaddr_in6 server_address_6;
    server_address_6.sin6_family = AF_INET6;
    server_address_6.sin6_port = htons(9000);
    inet_pton(AF_INET6, "127.0.0.1", &server_address_6.sin6_addr);

    int b = bind(network_socket, (sockaddr*)&server_address, sizeof(server_address));
    std::cout << b << std::endl;
    std::cout << strerror(errno) << std::endl;

    int b6 = bind(network_socket_6, (sockaddr*)&server_address_6, sizeof(server_address_6));
    std::cout << b6 << std::endl;
    std::cout << strerror(errno) << std::endl;

    std::string upstream_address = "8.8.8.8";

    sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    char buffer[1024];

    //int bytes_from_client = recvfrom(network_socket, buffer, 1024, 0, (sockaddr*)&INADDR_ANY, &client_size);
}