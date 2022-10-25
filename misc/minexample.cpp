#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9999
#define BUFLEN 4096
#define UPSTREAM_SERVER "8.8.8.8"
#define UPSTREAM_PORT 53

int main()
{
    int network_socket = socket(AF_INET, SOCK_DGRAM, 0);

    int no = 0;
    int i = setsockopt(network_socket, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)); 
    std::cout << i << std::endl;

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    bind(network_socket, (sockaddr*)&server_address, sizeof(server_address));

    char buf[BUFLEN];

    while (true)
    {
        memset(buf, 0, BUFLEN);
        sockaddr_in client_address;
        socklen_t clientSize = sizeof(client_address);

        int bytes_from_client = recvfrom(network_socket, buf, BUFLEN, 0, (sockaddr*)&client_address, &clientSize);

        sockaddr_in upstream_server;
        socklen_t upstream_server_size = sizeof(upstream_server);
        upstream_server.sin_family = AF_INET;
        upstream_server.sin_port = htons(UPSTREAM_PORT);
        inet_pton(AF_INET, UPSTREAM_SERVER, &(upstream_server.sin_addr));

        int network_socket_upstream = socket(AF_INET, SOCK_DGRAM, 0);
        i = setsockopt(network_socket_upstream, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)); 
        std::cout << i << std::endl;

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

        sendto(network_socket, buf, bytes_from_upstream + 1, 0, (sockaddr*)&client_address, clientSize);
    }

    close(network_socket);

    return 0;
}
