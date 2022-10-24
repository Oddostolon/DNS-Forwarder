#include "dns_forward_server.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9999

int dns_forward_server::init(std::string upstream_address, int upstream_port)
{
    int network_socket_client = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket_client == -1)
    {
        std::cerr << "Error: Can't create socket" << std::endl;
        return -1;
    }

    int network_socket_upstream = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket_upstream == -1)
    {
        std::cerr << "Error: Can't create socket" << std::endl;
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    upstream_server.sin_family = AF_INET;
    upstream_server.sin_port = htons(upstream_port);
    inet_pton(AF_INET, upstream_address.c_str(), &(upstream_server.sin_addr));

    return 0;
}

int dns_forward_server::send_message_from_buffer(int message_size, int sockaddr_size)
{
    return sendto
                (
                    network_socket_upstream, 
                    buffer, 
                    message_size, 
                    0, 
                    (sockaddr*)&upstream_server, 
                    sockaddr_size
                );
}

int dns_forward_server::receive_message_into_buffer(sockaddr_in sender_address, socklen_t sender_address_size)
{

    int bytes_received = recvfrom
                            (
                                network_socket_client, 
                                buffer, 
                                BUFLEN, 
                                0, 
                                (sockaddr*)&sender_address, 
                                &sender_address_size
                            );

    return bytes_received;
}

int dns_forward_server::run(std::string upstream_address, int upstream_port)
{
    network_socket_client = socket(AF_INET, SOCK_DGRAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    bind(network_socket_client, (sockaddr*)&server_address, sizeof(server_address));

    char buf[BUFLEN];

    while (true)
    {
        memset(buf, 0, BUFLEN);
        sockaddr_in client_address;
        socklen_t clientSize = sizeof(client_address);

        int bytes_from_client = recvfrom(network_socket_client, buf, BUFLEN, 0, (sockaddr*)&client_address, &clientSize);

        socklen_t upstream_server_size = sizeof(upstream_server);
        upstream_server.sin_family = AF_INET;
        upstream_server.sin_port = htons(upstream_port);
        inet_pton(AF_INET, upstream_address.c_str(), &(upstream_server.sin_addr));

        network_socket_upstream = socket(AF_INET, SOCK_DGRAM, 0);


        int relay_response = sendto
                                    (
                                        network_socket_upstream,
                                        buf,
                                        bytes_from_client + 1,
                                        0,
                                        (sockaddr*)&upstream_server,
                                        upstream_server_size
                                    );

        memset(buf, 0, BUFLEN);

        int bytes_from_upstream = recvfrom(network_socket_upstream, buf, BUFLEN, 0, (sockaddr*)&upstream_server, &upstream_server_size);

        sendto(network_socket_client, buf, bytes_from_upstream + 1, 0, (sockaddr*)&client_address, clientSize);
    }

    close(network_socket_client);
    close(network_socket_upstream);

    return 0;
}