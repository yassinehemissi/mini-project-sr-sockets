#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6000
#define FILENAME "./entr1_data.csv"
#define MESSAGE_MAX_SIZE 1024

void getData(char * data_string){
    FILE *file = fopen(FILENAME, "r");

    if (file == NULL) {
        perror("Error opening data file");
        exit(EXIT_FAILURE);
    }

    char line[1000];
    int row = 0;

    // Read each line from the file
    while (fgets(line, sizeof(line), file) != NULL) {
        strcat(data_string, line);
    }

    fclose(file);
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

    
    int socketDescriptor;
    struct sockaddr_in serverAddr, clientAddr;

    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor < 0)
        return handleError("Erreur lors de la création de la socket");
    configureServerAddr(&serverAddr);

    int bindingResult = bind(socketDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bindingResult < 0)
        return handleError("Erreur lors de la liaison de la socket");

    printf("Entr1 server waiting for message on port %d:\n", PORT); 

    socklen_t clientAddrSize; 

    while (1){
        char message[MESSAGE_MAX_SIZE]; 
        clientAddrSize = sizeof(clientAddr); 
        ssize_t bytesIn = recvfrom(socketDescriptor, message, sizeof(message), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesIn < 0) {
            continue;
        }
        message[bytesIn] = '\0'; 
        printf("Message received: %s\n", message);
        if (strcmp(message, "GETDATA") == 0){
            char data_string[1000000]; 
            getData(data_string); 
            ssize_t sentBytes = sendto(socketDescriptor, data_string, strlen(data_string), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
        } else {
            ssize_t sentBytes = sendto(socketDescriptor, "Unhandled Message", strlen("Unhandled Message"), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
        }

    }

    close(socketDescriptor); 

    return 0;
}