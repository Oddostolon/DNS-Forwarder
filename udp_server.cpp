#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "dns_forward_server.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9999
#define UPSTREAM_SERVER "8.8.8.8"
#define UPSTREAM_PORT 53
 
using namespace std;
 
int main(int argc, char* argv[])
{
    int upstream_port = 53;
    std::string upstream_address = "8.8.8.8";
    dns_forward_server server; 

    return server.run(upstream_address, upstream_port);
}