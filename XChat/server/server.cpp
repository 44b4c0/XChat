#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <vector>

#include "server.hpp"

int main(void){
    std::vector<Client> client_vector;
    int server_socket; sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_address.sin_port = htons(SERVER_PORT);

    socklen_t sizeof_server_address = sizeof(server_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0){
        return 1;
    }

    if(bind(server_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof_server_address) < 0){
        return 1;
    }

    if(listen(server_socket, 5) < 0){
        return 1;
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD* ssl_method = SSLv23_server_method();
    SSL_CTX* ssl_context = SSL_CTX_new(ssl_method);

    if(!ssl_context){
        return 1;
    }

    if (SSL_CTX_use_certificate_file(ssl_context, "server.crt", SSL_FILETYPE_PEM) <= 0 || SSL_CTX_use_PrivateKey_file(ssl_context, "server.key", SSL_FILETYPE_PEM) <= 0) {
        return 1;
    }

    return 0;
}