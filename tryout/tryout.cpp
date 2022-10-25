#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <bits/stdc++.h>

#define BUFLEN 1024

int main()
{
    std::regex ipv6("((([0-9a-fA-F]){1,4})\\:){7}([0-9a-fA-F]){1,4}");

    if(std::regex_match("2606:4700:4700::1111", ipv6))
    {
        std::cout << "It matched" << std::endl;
        return 0;
    }

    std::cout << "It didn't" << std::endl;
    return 0;
}