#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h> 

// --- Dump Data ---
#define NUM_PATIENT 2
int path1[] = {10, 20, 30};
int path2[] = {5, 15, 25, 35};

int* patient_paths[] = {path1, path2};
int path_sizes[] = {sizeof(path1) / sizeof(int), sizeof(path2) / sizeof(int)};
// ----------------------------------------------------

bool check_route(int get_route[], size_t route_len);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int route[100];
    int server_socket;
    int client_socket;
    ssize_t read_len;
    
    struct sockaddr_in server_add;
    struct sockaddr_in client_add;
    socklen_t client_add_size;

    //server_socket
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) { perror("socket() error"); exit(EXIT_FAILURE); }
    
    //server_initiate
    memset(&server_add, 0, sizeof(server_add));
    server_add.sin_family = AF_INET;  
    server_add.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 NIC에서 접속 허용
    server_add.sin_port = htons(atoi(argv[1]));     // 명령줄 인수로 포트 설정

    //binding
    if (bind(server_socket, (struct sockaddr*) &server_add, sizeof(server_add)) == -1) {
        perror("bind() error");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    //wait
    if (listen(server_socket, 5) == -1) { // 대기 큐 크기 5로 설정
        perror("listen() error");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    //commit
    printf("Server listening on port %s...\n", argv[1]);
    client_add_size = sizeof(client_add);
    client_socket = accept(server_socket, (struct sockaddr*)&client_add, &client_add_size);
    if (client_socket == -1) { perror("accept() error"); close(server_socket); exit(EXIT_FAILURE); }
    printf("Client connected: %s\n", inet_ntoa(client_add.sin_addr));

    //read
    //let client using route value
    read_len = read(client_socket, route, sizeof(route));
    if (read_len > 0) {
        printf("Received %zd bytes of route data.\n", read_len);
        //checking
        size_t received_ints = read_len / sizeof(int);
        bool match_found = check_route(route, received_ints);
        
        //bool -> char
        char result_char = match_found ? '1' : '0'; 
        
        if (write(client_socket, &result_char, sizeof(result_char)) == -1) {
            perror("write() error");
        } else {
            printf("Verification result sent: %c\n", result_char);
        }
    } else if (read_len == 0) {
        printf("Client closed connection.\n");
    } else {
        perror("read() error");
    }

    //close
    close(client_socket);
    close(server_socket);     

    return 0;
}

//route check function
bool check_route(int get_route[], size_t route_len)
{
    for(size_t i = 0; i < route_len; i++){
        for(int j = 0; j < NUM_PATIENT; j++){
            for(int k = 0; k < path_sizes[j]; k++){ 
                if(get_route[i] == patient_paths[j][k]) {
                    return true;
                }
            }
        }
    }
    return false;
}
