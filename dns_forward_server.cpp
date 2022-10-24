#include "dns_forward_server.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9999

int dns_forward_server::init(std::string upstream_address, int upstream_port)
{
    network_socket_client = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket_client == -1)
    {
        std::cerr << "Error creating client socket." << std::endl;
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    bind(network_socket_client, (sockaddr*)&server_address, sizeof(server_address));
    
    network_socket_upstream = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket_upstream == -1)
    {
        std::cerr << "Error creating upstream socket." << std::endl;
        return -1;
    }

    upstream_server.sin_family = AF_INET;
    upstream_server.sin_port = htons(upstream_port);
    inet_pton(AF_INET, upstream_address.c_str(), &(upstream_server.sin_addr));

    

    return 0;
}

int dns_forward_server::run()
{
    socklen_t upstream_server_size = sizeof(upstream_server);

    while (true)
    {
        memset(buffer, 0, BUFLEN);
        
        sockaddr_in client_address;
        socklen_t client_size = sizeof(client_address);

        int bytes_from_client = recvfrom(network_socket_client, buffer, BUFLEN, 0, (sockaddr*)&client_address, &client_size);
        if (bytes_from_client == -1)
        {
            std::cerr << "Error receiving client request." << std::endl;
            continue; 
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
            continue;
        }

        memset(buffer, 0, BUFLEN);

        int bytes_from_upstream = recvfrom(network_socket_upstream, buffer, BUFLEN, 0, (sockaddr*)&upstream_server, &upstream_server_size);
        if (bytes_from_upstream == -1)
        {
            std::cerr << "Error receiving upstream server response." << std::endl;
            continue;
        }

        int client_relay_response = sendto(network_socket_client, buffer, bytes_from_upstream + 1, 0, (sockaddr*)&client_address, client_size);
        if (client_relay_response == -1)
        {
            std::cerr << "Error forwarding upstream server response to client." << std::endl; 
            continue;
        }
    }

    close(network_socket_client);
    close(network_socket_upstream);

    return 0;
}