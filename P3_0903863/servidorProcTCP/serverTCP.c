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

void main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    int sd, sc;
    int size, val;
    int *shared_res;
    pid_t pid;
    shared_res = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_res == MAP_FAILED) {
        perror("Erreur avec mmap");
        exit(1);
    }
    int num[2];
    char operation;

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
        pid = fork();
        if (pid == 0) {
        close(sd);
        read(sc, &operation, sizeof(char)); // Leer operación
        read(sc, (char *)num, 2 * sizeof(int)); // Leer números
        switch (operation) {
            case '+':
                res = num[0] + num[1];
                break;
            case '-':
                res = num[0] - num[1];
                break;
            case '*':
                res = num[0] * num[1];
                break;
            case '/':
                if (num[1] != 0)
                    res = num[0] / num[1];
                else
                    res = 0; // Manejo básico de división por cero
                break;
            case '!':
                res = factorial(num[0]);
                break;
            default:
                res = 0; // Operación no válida
                break;
        }
        write(sc, shared_res, sizeof(int));
        close(sc);
        exit(0);
        } 
        else if (pid > 0) { 
            close(sc); 
            waitpid(-1, NULL, WNOHANG); 
        } 
        else {
            printf("Error en fork");
        }
    }
    close(sd);
    exit 0;
}

