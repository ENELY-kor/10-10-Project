#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

#define ARRAY_SIZE 100

//using for check result
bool val1 = false;
bool val2 = false;

void handle_route_exchange(int client_sock, int route[], bool *result_val, int route_num);

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP Address> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    //------------DUMP DATA---------------
    int route1[ARRAY_SIZE];
    int route2[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        route1[i] = 10 + i;  // {10, 11, 12, ...}
        route2[i] = 50 + i;  // {50, 51, 52, ...}
    }
    // -----------------------------------------------------------

    int client_socket;
    struct sockaddr_in server_add;
    
    // 1. make socket
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) { perror("socket() error"); exit(EXIT_FAILURE); }

    // 2. set server
    memset(&server_add, 0, sizeof(server_add));
    server_add.sin_family = AF_INET;  
    
    // 3. get IP
    if (inet_pton(AF_INET, argv[1], &server_add.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/ Address not supported \n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    // 4. get Port
    server_add.sin_port = htons(atoi(argv[2]));
    
    // 5. connect to host
    if (connect(client_socket, (struct sockaddr*) &server_add, sizeof(server_add)) == -1) {
        perror("connect() error");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server %s:%s\n", argv[1], argv[2]);

    // check route 1
    handle_route_exchange(client_socket, route1, &val1, 1);
    
    // check route 2
    handle_route_exchange(client_socket, route2, &val2, 2);
    
    // close socket
    close(client_socket);

    printf("\n--- Final Results ---\n");
    printf("Route 1 Verification Result: %s\n", val1 ? "MATCHED (1)" : "NOT MATCHED (0)");
    printf("Route 2 Verification Result: %s\n", val2 ? "MATCHED (1)" : "NOT MATCHED (0)");
    
    return (val1 || val2) ? 1 : 0; 
}

void handle_route_exchange(int client_sock, int route[], bool *result_val, int route_num) {
    char server_response;
    ssize_t read_len;

    printf("\n[Route %d] Sending %zu bytes of data...\n", route_num, sizeof(int) * ARRAY_SIZE);

    if (write(client_sock, route, sizeof(int) * ARRAY_SIZE) == -1) {
        perror("write() error");
        return;
    }
    
    read_len = read(client_sock, &server_response, sizeof(server_response));
    
    if (read_len == 1) {
        *result_val = (server_response == '1');
        printf("[Route %d] Server Response: %c -> Result: %s\n", 
               route_num, server_response, *result_val ? "MATCH" : "NO MATCH");
    } else if (read_len == 0) {
        printf("[Route %d] Connection closed by server before receiving response.\n", route_num);
    } else {
        perror("[Route %d] read() error");
    }
}
