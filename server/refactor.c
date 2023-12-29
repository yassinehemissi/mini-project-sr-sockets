#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>

#define MAX_MESSAGE_SIZE 1024
#define PORT 14587

int connectToEntr2()
{
    int socketDescriptor;
    struct sockaddr_in serverAddr;
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6002);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    while (connect(socketDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        sleep(1);
    return socketDescriptor;
}

void connectToEntr1(char *data)
{
    struct sockaddr_in serverAddr;
    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    ssize_t sent_len = sendto(socketDescriptor, "GETDATA", strlen("GETDATA"), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    socklen_t serverAddLength = sizeof(serverAddr);
    recvfrom(socketDescriptor, data, 1000000, 0, (struct sockaddr *)&serverAddr, &serverAddLength);
}

int getRecetteEntr(char *data, int mpos, char *finishMessage)
{
    int montant = 0;
    int lp = 0;
    char montant_str[1000] = "";
    int ln = 0;
    for (int i = 0; i < strlen(data); i++)
    {
        if (data[i] == '\n')
        {
            if (strcmp(montant_str, finishMessage) == 0)
                return montant;
            montant += atoi(montant_str);
            strcpy(montant_str, "");
            ln++;
            lp = 0;
        }
        if (ln == 0)
            continue;
        if (lp == mpos)
            strcat(montant_str, (char[2]){(char)data[i], '\0'});
        if (data[i] == '|')
            lp++;
    }
    return montant;
}

void handleClient(int clientSocketDescriptor, int entr2SocketDescriptor)
{

    int connection = 1;
    int requestType;
    int bytesIn;
    char message[MAX_MESSAGE_SIZE];
    char data[1000000];
    int recette; 
    while ((bytesIn = recv(clientSocketDescriptor, message, MAX_MESSAGE_SIZE * sizeof(char), 0)) > 0)
    {
        if (!connection)
            break;
        requestType = message[0] - '0';
        if (requestType > 2 || requestType < 0)
            continue;
        char message[1000000];
        printf("REQUEST TYPE %d\n", requestType);
        switch (requestType)
        {
        case 1:
            strcpy(data, "");
            connectToEntr1(data);
            printf("%s\n", data); 
            recette = getRecetteEntr(data, 2, "montant");
            sprintf(message, "%d", recette);
            write(clientSocketDescriptor, message, strlen(message));
            break;
        case 2:
            strcpy(data, "");
            write(entr2SocketDescriptor, "REQUEST", strlen("REQUEST"));
            read(entr2SocketDescriptor, data, 1000000);
            recette = getRecetteEntr(data, 6, "totale");
            sprintf(message, "%d", recette);
            write(clientSocketDescriptor, message, strlen(message));
            break;
        case 0:
            connection = 0;
            break;
        default:
            break;
        }
    }
}

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

int main()
{
    int entr2 = connectToEntr2();
    int socketDescriptor, clientSocketDescriptor;
    struct sockaddr_in serverAddr, clientAddr;
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0)
        return handleError("Erreur lors de la création de la socket");
    configureServerAddr(&serverAddr);
    int bindingResult = bind(socketDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bindingResult < 0)
        return handleError("Erreur lors de la liaison de la socket");

    int listeningResult = listen(socketDescriptor, 5);
    if (listeningResult < 0)
        return handleError("Erreur lors de l'ecoute de la socket.");

    socklen_t clientSocketAddrLength = sizeof(struct sockaddr_in);

    printf("Server waiting for connection on port %d:\n", PORT);

    while (1)
    {
        int clientSocketDescriptor = accept(socketDescriptor, (struct sockaddr *)&clientAddr, &clientSocketAddrLength);
        if (clientSocketDescriptor < 0)
        {
            close(clientSocketDescriptor);
            printf("Connection Failed\n");
            continue;
        }
        if (fork() == 0)
        {
            close(socketDescriptor);
            handleClient(clientSocketDescriptor, entr2);
        }
        else
        {
            close(clientSocketDescriptor);
        }
    }

    return 0;
}