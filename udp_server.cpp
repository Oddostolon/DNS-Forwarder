#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <bits/stdc++.h>
#include "dns_forward_server.h"
 
using namespace std;

int main(int argc, char* argv[])
{
    dns_forward_server server; 

    if (argc != 3)
    {
        std::cerr << "Error: This program takes 2 arguments." << std::endl;
        return -1;
    }

    std::string port = argv[2];

    if(port.empty() || !std::all_of(port.begin(), port.end(), ::isdigit))
    {
        std::cerr << "Error: Please enter a valid port number" << std::endl;
        return -1;
    }

    int init_result = server.init(argv[1], stoi(argv[2]));
    if (init_result == -1)
    {
        std::cerr << "Initializing server failed, aborting." << std::endl;
        return -1;
    }

    return server.run();
}