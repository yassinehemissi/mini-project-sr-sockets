#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASSWORD "root"
#define DB_NAME "car_database"

void finish_with_error(MYSQL *conn)
{
    fprintf(stderr, "Error: %s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

void getData(char * data_string)
{
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        return;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL)
    {
        finish_with_error(conn);
    }

    if (mysql_query(conn, "SELECT autos.id, autos.dateV, marque.marque, autos.modele, autos.nbr, autos.prixU FROM autos INNER JOIN marque ON autos.marque = marque.id;"))
    {
        finish_with_error(conn);
    }

    MYSQL_RES *result = mysql_store_result(conn);

    if (result == NULL)
    {
        finish_with_error(conn);
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    strcat(data_string, "id|date|marque|modele|nbr_vente|punite|totale\n");
    while ((row = mysql_fetch_row(result)))
    {
        int nbr; 
        int price; 
        for (int i = 0; i < num_fields; i++)
        {
            if (i == num_fields -2){
                nbr = atoi(row[i]); 
            }
            if (i == num_fields - 1){
                price = atoi(row[i]); 
            }
            strcat(data_string, row[i]);
            strcat(data_string, "|");
        }
        int montant = nbr * price; 
        char montant_str[1000000]; 
        sprintf(montant_str, "%d", montant); 
        strcat(data_string, montant_str);
        strcat(data_string, "\n");
    }
    mysql_free_result(result);
    mysql_close(conn);

}