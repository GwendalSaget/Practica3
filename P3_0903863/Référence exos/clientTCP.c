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
#include <netdb.h>

void main {
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int num[2], res;
    char operation;
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bzero((char *)&server_addr, sizeof(server_addr));
    hp = gethostbyname("localhost");
    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4200);
    connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Ingrese la operación (+, -, *, /, !): ");
    scanf(" %c", &operation);

    if (operation == '!') {
        printf("Ingrese un número: ");
        scanf("%d", &num[0]);
        num[1] = 0;
    } 
    else {
        printf("Ingrese el primer número: ");
        scanf("%d", &num[0]);
        printf("Ingrese el segundo número: ");
        scanf("%d", &num[1]);
    }
    write(sd, &operation, sizeof(char)); 
    write(sd, (char *)num, 2 * sizeof(int)); 
    read(sd, &res, sizeof(int));
    printf("Resultado: %d\n", res);
    close(sd);
    return 0;
}
