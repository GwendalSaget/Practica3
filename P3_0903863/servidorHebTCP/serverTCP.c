#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Función para calcular el factorial
int factorial(int n) {
    if (n < 0) return -1; // No se define para negativos
    if (n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}
typedef struct {
    int client_socket;
} ThreadArgs;

void *handle_client(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int client_socket = thread_args->client_socket;
    free(thread_args); 
    char operation;
    int num[2], result;
    read(client_socket, &operation, sizeof(char));
    read(client_socket, num, 2 * sizeof(int));     
    switch (operation) {
        case '+':
            result = num[0] + num[1];
            break;
        case '-':
            result = num[0] - num[1];
            break;
        case '*':
            result = num[0] * num[1];
            break;
        case '/':
            result = (num[1] != 0) ? (num[0] / num[1]) : 0; 
            break;
        case '!':
            result = factorial(num[0]);
            break;
        default:
            result = 0; 
            break;
    }
    write(client_socket, &result, sizeof(int));
    close(client_socket);
    pthread_exit(NULL);
}

void main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    int sd, sc;
    int size, val;
    int num[2], res;
    char operation;
    pthread_t thread_id;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4200);

    bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(sd, 5);
    size = sizeof(client_addr);

    while (1) {
        printf("Esperando conexión...\n");
        sc = accept(sd, (struct sockaddr *)&client_addr, &size);
        rprintf("Conexion acceptada\n");
        ThreadArgs *thread_args = malloc(sizeof(ThreadArgs));
        if (thread_args == NULL) {
            perror("Error de memoria");
            close(sc);
            continue;
        }
        thread_args->client_socket = sc;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)thread_args) != 0) {
            perror("Error en pthread create");
            free(thread_args);
            close(sc);
            continue;
        }
        pthread_detach(thread_id);
    }
    close(sd);
    return 0;
}

