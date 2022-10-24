#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <bits/stdc++.h>
#include "dns_forward_server.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define UPSTREAM_SERVER "8.8.8.8"
#define UPSTREAM_PORT 53
 
using namespace std;

int main(int argc, char* argv[])
{
    dns_forward_server server; 

    regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
    regex ipv6("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");

    if (argc != 3)
    {
        std::cerr << "Error: This program takes 2 arguments." << std::endl;
        return -1;
    }

    if (!regex_match(argv[1], ipv4) && !regex_match(argv[1], ipv6))
    {
        std::cerr << "Error: Please enter a valid Ipv4 or Ipv6 address." << std::endl;
        return -1;
    }

    std::string port = argv[2];

    if(port.empty() || !std::all_of(port.begin(), port.end(), ::isdigit))
    {
        std::cerr << "Error: Please enter a valid port number" << std::endl;
        return -1;
    }

    server.init(argv[1], stoi(argv[2]));

    return server.run();
}