#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <vector>
#include <thread>

#include "server.hpp"

int main(void){
    std::mutex client_vector_mutex;
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

    if (SSL_CTX_use_certificate_file(ssl_context, "keys/server.crt", SSL_FILETYPE_PEM) <= 0 || SSL_CTX_use_PrivateKey_file(ssl_context, "keys/server.key", SSL_FILETYPE_PEM) <= 0) {
        return 1;
    }

    while(true == true){
        if(client_vector.size() >= MAX_CLIENT){
            SSL* ssl_fd = SSL_new(ssl_context);

            if(!ssl_fd){
                SSL_free(ssl_fd);
                continue;
            }

            sockaddr_in client_address;
            socklen_t client_address_size;
            int client_fd = accept(server_socket, reinterpret_cast<sockaddr*>(&client_address), &client_address_size);

            if(client_fd < 0){
                SSL_free(ssl_fd);
                close(client_fd);
                continue;
            }

            char username_buffer[USERNAME_SIZE];
            int recevied_bytes = SSL_read(ssl_fd, username_buffer, USERNAME_SIZE - 1);

            if(recevied_bytes <= 0){
                SSL_free(ssl_fd);
                close(client_fd);
                continue;
            }

            username_buffer[recevied_bytes] = '\0';

            SSL_write(ssl_fd, CODE_SERVER_FULL, strlen(CODE_SERVER_FULL));

            SSL_free(ssl_fd);
            close(client_fd);
            continue;
        }
        else{
            SSL* ssl_fd = SSL_new(ssl_context);

            if(!ssl_fd){
                SSL_free(ssl_fd);
                continue;
            }

            sockaddr_in client_address;
            socklen_t client_address_size;
            int client_fd = accept(server_socket, reinterpret_cast<sockaddr*>(&client_address), &client_address_size);

            if(client_fd < 0){
                close(client_fd);
                continue;
            }

            SSL_set_fd(ssl_fd, client_fd);

            if(SSL_accept(ssl_fd) < 0 ){
                SSL_free(ssl_fd);
                close(client_fd);
                continue;
            }

            char username_buffer[USERNAME_SIZE];

            int received_bytes = SSL_read(ssl_fd, username_buffer, USERNAME_SIZE - 1);

            if(received_bytes <= 0){
                SSL_free(ssl_fd);
                close(client_fd);
                continue;
            }

            username_buffer[received_bytes] = '\0';

            std::thread new_thread(ReadFromClient, ssl_fd, client_fd, client_address, username_buffer, std::ref(client_vector_mutex), std::ref(client_vector));
            new_thread.detach();
        }
    }

    return 0;
}
