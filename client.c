#include <stdio.h> /* fichiers d'en-tête classiques */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> /* fichiers d'en-tête "réseau" */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define BUFFER_SIZE 1024 /* Taille maximum des messages c'est l'équivalent de MAX chez le serveur*/
#define PORT 6005

void flush_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(int argc, char *argv[])
{
    int sclient;
    int con = 1;
    struct sockaddr_in saddr = {0};
    sclient = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;
    while (connect(sclient, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
        sleep(1);
    do
    {
        char commande[BUFFER_SIZE] = {0}, cc, message[BUFFER_SIZE] = {0};
        printf("Entrer le commande pour interogger le serveur:\n1-Recette Entr1\n2-Recette Entr2\n3-Data Entr1\n4- Data Entr2\n0-Close\n");
        scanf("%c", &cc);
        commande[0] = cc;
        commande[1] = '\0';
        if (cc == '0')
        {
            con = 0;
            continue;
        }
        else if (cc > '4' || cc < '0')
        {
            continue;
        }
        int re = write(sclient, commande, strlen(commande) + 1); /* Envoi de la commande */
        read(sclient, message, 1000000);
        printf("Message du serveur: %s\n", message);
        flush_input_buffer();
    } while (con);

    shutdown(sclient, SHUT_RDWR);
    close(sclient);
    return 0;
}