#pragma once

#include <openssl/ssl.h>

#define SERVER_ADDRESS "0.0.0.0"
#define SERVER_PORT 8080

class Client {
    public:
    char* username;

    char* ip_address;
    unsigned int port;

    int client_fd;
    SSL* ssl_fd;
};