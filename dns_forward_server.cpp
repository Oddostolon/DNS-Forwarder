#include "dns_forward_server.h"
#include <poll.h>
#include <iostream>
#include <bits/stdc++.h>
#include <signal.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_IP_6 "::1"
#define SERVER_PORT 9000


int dns_forward_server::init_ipv4_resources()
{
    network_socket_client = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket_client == -1)
    {
        std::cerr << "Error creating IPv4 client socket." << std::endl << strerror(errno) << std::endl;
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    int bind_result = bind(network_socket_client, (sockaddr*)&server_address, sizeof(server_address));
    if (bind_result == -1)
    {
        std::cerr << "Binding IPv4 socket failed: " << std::endl << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

int dns_forward_server::init_ipv6_resources()
{
    network_socket_client_6 = socket(AF_INET6, SOCK_DGRAM, 0);
    if (network_socket_client == -1)
    {
        std::cerr << "Error creating IPv6 client socket." << std::endl << strerror(errno) << std::endl;
        return -1;
    }

    server_address_6.sin6_family = AF_INET6;
    server_address_6.sin6_port = htons(SERVER_PORT);
    inet_pton(AF_INET6, SERVER_IP_6, &server_address_6.sin6_addr);

    int bind_result = bind(network_socket_client_6, (sockaddr*)&server_address_6, sizeof(server_address_6));
    if (bind_result == -1)
    {
        std::cerr << "Binding IPv6 socket failed: " << std::endl << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

int dns_forward_server::init_ipv4_upstream_resources(std::string upstream_address, int upstream_port)
{
    is_ipv4 = 1; 
    
    network_socket_upstream = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket_upstream == -1)
    {
        std::cerr << "Error creating upstream IPv4 socket." << std::endl << strerror(errno) << std::endl;
        return -1;
    }

    upstream_server.sin_family = AF_INET;
    upstream_server.sin_port = htons(upstream_port);
    inet_pton(AF_INET, upstream_address.c_str(), &(upstream_server.sin_addr));

    return 0;
}

int dns_forward_server::init_ipv6_upstream_resources(std::string upstream_address, int upstream_port)
{
    is_ipv4 = 0;

    network_socket_upstream_6 = socket(AF_INET6, SOCK_DGRAM, 0);
    if (network_socket_upstream == -1)
    {
        std::cerr << "Error creating upstream IPv6 socket." << std::endl << strerror(errno) << std::endl;
        return -1;
    }

    upstream_server_6.sin6_family = AF_INET6;
    upstream_server_6.sin6_port = htons(upstream_port);
    inet_pton(AF_INET, upstream_address.c_str(), &(upstream_server_6.sin6_addr));

    return 0;
}

int dns_forward_server::init(std::string upstream_address, int upstream_port)
{
    is_ipv4 = -1;

    std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");

    if (std::regex_match(upstream_address, ipv4))
    {
        int init_upstream_result = init_ipv4_upstream_resources(upstream_address, upstream_port);
        if(init_upstream_result == -1)
        {
            return -1;
        }
    }

    int init_upstream_result = init_ipv6_upstream_resources(upstream_address, upstream_port);
    if (init_upstream_result == -1)
    {
        return -1;
    }

    int init_result = init_ipv4_resources();
    if (init_result == -1)
    {
        return -1; 
    }

    int init_result_6 = init_ipv6_resources();
    if (init_result_6 == -1)
    {
        return -1;
    }

    return 0;
}

int dns_forward_server::forward_ipv4_request()
{
    socklen_t upstream_server_size = sizeof(upstream_server);

    sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    int bytes_from_client = recvfrom(network_socket_client, buffer, BUFLEN, 0, (sockaddr*)&client_address, &client_size);
    if (bytes_from_client == -1)
    {
        std::cerr << "Error receiving client request." << std::endl;
        return -1; 
    }
    
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
        return -1;
    }

    memset(buffer, 0, BUFLEN);

    int bytes_from_upstream = recvfrom(network_socket_upstream, buffer, BUFLEN, 0, (sockaddr*)&upstream_server, &upstream_server_size);
    if (bytes_from_upstream == -1)
    {
        std::cerr << "Error receiving upstream server response." << std::endl;
        return -1;
    }

    int client_relay_response = sendto(network_socket_client, buffer, bytes_from_upstream + 1, 0, (sockaddr*)&client_address, client_size);
    if (client_relay_response == -1)
    {
        std::cerr << "Error forwarding upstream server response to client." << std::endl; 
        return -1;
    }

    return 0;
}

int dns_forward_server::forward_ipv6_request() 
{
    socklen_t upstream_server_size = sizeof(upstream_server_6);

    sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    int bytes_from_client = recvfrom(network_socket_client_6, buffer, BUFLEN, 0, (sockaddr*)&client_address, &client_size);
    if (bytes_from_client == -1)
    {
        std::cerr << "Error receiving client request." << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return -1; 
    }
    
    int upstream_relay_response = sendto
                            (
                                network_socket_upstream_6,
                                buffer,
                                bytes_from_client + 1,
                                0,
                                (sockaddr*)&upstream_server_6,
                                upstream_server_size
                            );
    if (upstream_relay_response == -1)
    {
        std::cerr << "Error forwarding client request to upstream server." << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return -1;
    }

    memset(buffer, 0, BUFLEN);

    int bytes_from_upstream = recvfrom(network_socket_upstream_6, buffer, BUFLEN, 0, (sockaddr*)&upstream_server_6, &upstream_server_size);
    if (bytes_from_upstream == -1)
    {
        std::cerr << "Error receiving upstream server response." << std::endl;
        std::cerr << strerror(errno) << std::endl;
        return -1;
    }

    int client_relay_response = sendto(network_socket_client, buffer, bytes_from_upstream + 1, 0, (sockaddr*)&client_address, client_size);
    if (client_relay_response == -1)
    {
        std::cerr << "Error forwarding upstream server response to client." << std::endl; 
        std::cerr << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

void dns_forward_server::signal_handler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        std::cout << "Interrupt signal received, shutting down." << std::endl;
        exit(signal);
    }
}

int dns_forward_server::run()
{
    struct sigaction signal_action;
    signal_action.sa_handler = &signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    if (sigaction(SIGINT, &signal_action, NULL) != 0) 
    {
        std::cerr << "Failed to register signal handler:" << std::endl << strerror(errno) << std::endl;
    }

    socklen_t upstream_server_size = sizeof(upstream_server);

    pollfd pfds[2];

    pfds[0].fd = network_socket_client;
    pfds[0].events = POLLIN;

    pfds[1].fd = network_socket_client_6;
    pfds[1].fd = POLLIN;

    int fd_count = 2;

    for(;;)
    {
        memset(buffer, 0, BUFLEN);

        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1)
        {
            std::cerr << "polls failed" << std::endl; 
            exit(1);
        }

        for (int i = 0; i < fd_count; i++)
        {
            if (pfds[i].fd == network_socket_client)
            {
                int forwarding_result = forward_ipv4_request();
                if (forwarding_result == -1)
                {
                    return -1;
                }
            }

            if (pfds[i].fd == network_socket_client_6)
            {
                int forwarding_result = forward_ipv6_request();
                if (forwarding_result == -1)
                {
                    return -1;
                }
            }
        }
    }

    close(network_socket_client);
    close(network_socket_upstream);

    return 0;
}