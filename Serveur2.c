#include <stdio.h>      // pour printf() et fprintf()
#include <sys/socket.h> // pour socket(), bind()
#include <arpa/inet.h>  // pour htons et htonl
#include <stdlib.h>     // pour atoi(), exist() 
#include <string.h>     // pour memset(), strcpy()  
#include <unistd.h>     // pour close()  
#include <netinet/in.h> // pour struct sockaddr_in
#include <netdb.h> // pour hostent
#include <my_global.h>
#include <mysql.h> // for mysql stuff

#define MAX   1024       // longueur du Buffer

// compile with this command gcc -o Serveur2 $(mysql_config --cflags) Serveur2.c $(mysql_config --libs)

// serveur de port 3030

int socketDialogue;
//return rows number
int interrogation_bd(char* id, char table_response[MAX*MAX]) {


    MYSQL *con = mysql_init(NULL);
    char requete[100];
    if (con==NULL) {
        fprintf(stderr, "%s\n" ,mysql_error(con));
        exit(-1);
    }
    if (mysql_real_connect(con,"localhost","faten","facturesdbpw","fatendb",0,NULL,0)==NULL) {
        fprintf(stderr,"%s\n",mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    sprintf(requete,"SELECT * FROM facture where idclient=%s;",id);
    if (mysql_query(con,requete)) {
        fprintf(stderr, "%s\n" ,mysql_error(con));
        exit(-1);
    }
    MYSQL_RES *result = mysql_store_result(con);
    if (result==NULL) {
        fprintf(stderr, "%s\n" ,mysql_error(con));
        exit(-1);
    }
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    row=mysql_fetch_row(result);
    int itr = 0;
    if (row==NULL) {
        // printf("Il n'existe pas de client portant l'identifiant: %s",id);
        sprintf(table_response, "Il n'existe pas de client portant l'identifiant: %s",id);
        // send(socketDialogue, table_response[0], strlen(table_response[0]),0);
        return 0;
    }
    else {
        // printf("Les factures du client ayant pour id %s:\n",id);
        char ligne[MAX];
        ligne[0] = '\0';
        table_response[0]='\0';
        int somme=0;
        for (int i=0;i<num_fields;i++) {
            strcat(ligne,row[i]);
            strcat(ligne,"\t\t");
        }
        somme += atoi(row[1]);

        strcat(ligne,"\n");
        itr++;
        
        while (row=mysql_fetch_row(result)) {
            somme += atoi(row[1]);
            for (int i=0;i<num_fields;i++) {
                strcat(ligne,row[i]);
                strcat(ligne,"\t\t");
            }
            strcat(ligne,"\n");
        }
        strcpy(table_response,ligne);
                char ft[MAX]="";
                char forma[MAX]="";
                sprintf(forma,"**************************\n");
                sprintf(ft,   "* La Somme:%d          *\n",somme);
                strcat(forma,ft);
                strcat(forma,"**************************\n");
                strcat(table_response,forma); 
    }
        
    mysql_free_result(result);
    mysql_close(con);
    return itr;
}

int main(int argc, char*argv[]) {
    
    char Buffer[MAX];
    char  table_response[MAX*MAX];
    socklen_t  longueurAdresse; 
    int socketServeur2,socketDialogue;
    struct sockaddr_in ServAddr;
    struct sockaddr_in ClientAddr;
    unsigned short ServPort;
    int countr,id;

    ServPort = atoi("3030");
    socketServeur2 = socket(AF_INET,SOCK_STREAM,0);
    if (socketServeur2<0) {
        perror("Problème de création de socket\n");
        return(-1);
    }
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&ServAddr,0x00,longueurAdresse);
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ServAddr.sin_port = htons(ServPort);

    if ((bind(socketServeur2,(struct sockaddr*)&ServAddr,longueurAdresse)) <0) {
        perror("bind");
        return -1;
    }

       if(listen(socketServeur2,5)  < 0) 
   { 
      perror("listen"); 
      exit(-1); 
   } 

    
    while (1) {
        printf("\nServeur Entr2 en attente de demande de connexion\n");
        memset(table_response,0,sizeof(table_response));
        socketDialogue = accept(socketServeur2,(struct sockaddr*)&ClientAddr,&longueurAdresse);
        if (socketDialogue<0) {
            perror("accept");
            close(socketDialogue);
            return(-1);
        }
        memset(Buffer,0x00,MAX*sizeof(char));
        memset(table_response,0x00,MAX*MAX*sizeof(char));
        countr = recv(socketDialogue,Buffer,MAX*MAX*sizeof(char),0);
        switch (countr)
        {
        case -1:
            perror("recv");
            close(socketDialogue);
            return -1;
            break;
        case 0:
            fprintf(stderr,"La socket a été fermée par le client!\n\n"); 
            return 0;
        default:
            printf("message reçu %s\n",Buffer);
            // memset(table_response,0,sizeof(table_response));
            int row_nbr = interrogation_bd(Buffer, table_response);
            
            if (row_nbr==0) {
                send(socketDialogue,"Pas de client avec cet id \n",MAX*MAX*sizeof(char),0);
            }
            else {
                puts(table_response);
                send(socketDialogue,table_response,strlen(table_response),0);
            }
            
        }
        close(socketDialogue);
    }
    close(socketServeur2);

    return 0 ; 
}