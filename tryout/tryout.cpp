#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <poll.h>

#define BUFLEN 1024

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
    inet_pton(AF_INET6, "::1", &server_address_6.sin6_addr);

    int b = bind(network_socket, (sockaddr*)&server_address, sizeof(server_address));
    std::cout << b << std::endl;
    std::cout << strerror(errno) << std::endl;

    int b6 = bind(network_socket_6, (sockaddr*)&server_address_6, sizeof(server_address_6));
    std::cout << b6 << std::endl;
    std::cout << strerror(errno) << std::endl;

    pollfd pfds[2];

    pfds[0].fd = network_socket;
    pfds[0].events = POLLIN;

    pfds[1].fd = network_socket_6;
    pfds[1].events = POLLIN; 

    int fd_count = 2;

    for(;;)
    {
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1)
        {
            std::cerr << "polls failed" << std::endl; 
            exit(1);
        }

        for (int i = 0; i< fd_count; i++)
        {
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == network_socket)
                {
                    std::cout << "IPv4 found!" << std::endl;

                    sockaddr_in client_address;
                    socklen_t client_size = sizeof(client_address);

                    char buffer[BUFLEN];

                    int bytes_from_client = recvfrom(network_socket, buffer, BUFLEN, 0, (sockaddr*)&client_address, &client_size);

                    auto network_socket_upstream = socket(AF_INET, SOCK_DGRAM, 0);

                    sockaddr_in upstream_server;
                    upstream_server.sin_family = AF_INET;
                    upstream_server.sin_port = htons(53);
                    inet_pton(AF_INET, "8.8.8.8", &(upstream_server.sin_addr));

                    socklen_t upstream_server_size = sizeof(upstream_server);

                    int upstream_relay_response = sendto
                                (
                                    network_socket_upstream,
                                    buffer,
                                    bytes_from_client + 1,
                                    0,
                                    (sockaddr*)&upstream_server,
                                    upstream_server_size
                                );
                    if (upstream_relay_response == -1)
                    {
                        std::cerr << "Error forwarding client request to upstream server." << std::endl;
                        continue;
                    }

                    memset(buffer, 0, 1024);

                    int bytes_from_upstream = recvfrom(network_socket_upstream, buffer, BUFLEN, 0, (sockaddr*)&upstream_server, &upstream_server_size);
                    if (bytes_from_upstream == -1)
                    {
                        std::cerr << "Error receiving upstream server response." << std::endl;
                        continue;
                    }

                    int client_relay_response = sendto(network_socket, buffer, bytes_from_upstream + 1, 0, (sockaddr*)&client_address, client_size);
                    if (client_relay_response == -1)
                    {
                        std::cerr << "Error forwarding upstream server response to client." << std::endl; 
                        continue;
                    }

                    std::cout << buffer << std::endl;
                }

                if (pfds[i].fd == network_socket_6)
                {
                    std::cout << "IPv6 found!" << std::endl;
                }
            }
        }
    }
}