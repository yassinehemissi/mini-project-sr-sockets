#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include "db.c"

#define MAX_MESSAGE_SIZE 1024
#define PORT 6002

int handleError(char *errorMessage)
{
    perror(errorMessage);
    exit(EXIT_FAILURE);
    return 0;
}

void configureServerAddr(struct sockaddr_in *serverAddr)
{
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(PORT);
    serverAddr->sin_addr.s_addr = INADDR_ANY;
}

void handleClient(int clientSocketDescriptor)
{
    char message[MAX_MESSAGE_SIZE];
    int bytesIn;
    while ((bytesIn = recv(clientSocketDescriptor, message, MAX_MESSAGE_SIZE * sizeof(char), 0)) > 0)
    {
        if(bytesIn <= 0) continue; 
        printf("%s\n", message);
        printf("Connection Established!\n");
        if (strcmp(message, "CLOSE") == 0)
        {
            printf("CONNECTION OFF\n");
            break; 
        }
        else if (strcmp(message, "REQUEST") == 0)
        {
            printf("CONNECTION 2\n");
            char data_all[1000000];
            strcpy(data_all, ""); 
            getData(data_all);
            printf("%s\n\n", data_all); 
            write(clientSocketDescriptor, data_all, strlen(data_all));
        }
    }
}

int main()
{
    int socketDescriptor, clientSocketDescriptor;
    struct sockaddr_in serverAddr, clientAddr;
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (socketDescriptor < 0)
        return handleError("Erreur lors de la création de la socket");
    configureServerAddr(&serverAddr);
    int bindingResult = bind(socketDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bindingResult < 0)
        return handleError("Erreur lors de la liaison de la socket");
    printf("Entr1 server waiting for connection on port %d:\n", PORT);

    int listeningResult = listen(socketDescriptor, 5);
    if (listeningResult < 0)
        return handleError("Erreur lors de l'ecoute de la socket.");

    socklen_t clientSocketAddrLength = sizeof(struct sockaddr_in);

    while (1)
    {
        clientSocketDescriptor = accept(socketDescriptor, (struct sockaddr *)&clientAddr, &clientSocketAddrLength);
        if (clientSocketDescriptor < 0)
            return handleError("Error Binding to a client!\n");

        if (fork() == 0)
        {
            close(socketDescriptor);
            handleClient(clientSocketDescriptor);
        }
        else
        {
            close(clientSocketDescriptor);
        }
    }
    close(socketDescriptor);

    return 0;
}