#pragma once

#include <mutex>
#include <algorithm>

#include <openssl/ssl.h>

/* Server Bind config */

#define SERVER_ADDRESS "0.0.0.0"
#define SERVER_PORT 8080

/* Server Logic config */
#define MAX_CLIENT 200

/* Communication config */
#define USERNAME_SIZE 32
#define MESSAGE_SIZE 1024

/* Code config */
#define CODE_SERVER_FULL "//FULL//"

class Client {
    public:
    char* username;

    char* ip_address;
    unsigned int port;

    int client_fd;
    SSL* ssl_fd;
};

void RemoveClient(std::vector<Client>& client_vector, int client_fd, std::mutex& client_vector_mutex) {
    std::lock_guard<std::mutex> lock(client_vector_mutex);

    client_vector.erase(
        std::remove_if(client_vector.begin(), client_vector.end(),
            [client_fd](const Client& client) {
                return client.client_fd == client_fd;
            }),
        client_vector.end());
}

void ReadFromClient(SSL* ssl_fd, int client_fd, sockaddr_in client_address, char* username, std::mutex& client_vector_mutex, std::vector<Client>& client_vector){
    {
        std::lock_guard<std::mutex> lock(client_vector_mutex);

        Client new_client;

        new_client.client_fd = client_fd;
        new_client.ssl_fd = ssl_fd;
        new_client.username = username;
        new_client.ip_address = inet_ntoa(client_address.sin_addr);
        new_client.port = ntohs(client_address.sin_port);

        client_vector.push_back(new_client);  
    }  

    while(true == true){
        char message_buffer[MESSAGE_SIZE];

        int received_bytes = SSL_read(ssl_fd, message_buffer, MESSAGE_SIZE - 1);

        if(received_bytes <= 0){
            continue;
        }
        else{
            message_buffer[received_bytes] = '\0';

            {
                std::lock_guard<std::mutex> lock(client_vector_mutex);

                for(const auto& client : client_vector){
                    SSL_write(client.ssl_fd, message_buffer, MESSAGE_SIZE);
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(client_vector_mutex);
        RemoveClient(client_vector, client_fd, client_vector_mutex);
    }
}
