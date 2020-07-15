#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdbool.h>
#include<netinet/in.h>      


#define max_size 4096
#define ip_address "127.0.0.1"
#define port_si1 2058


int  main(int argc, char * argv[]) 
{ 
    int  sserveur;                   
    struct sockaddr_in sadr;
    struct sockaddr_in cadr;
    unsigned int cadrlen;  
    char menv1[max_size];
    char mrec1[max_size]; 
    FILE* fichier = NULL;
    char chaine[max_size] = "";
    char s[max_size] = "";
    char st[max_size] = "";
    bool find=0;            
    int  countr,counts,longueurAdresse;                  
   
    sserveur = socket(AF_INET,SOCK_DGRAM,0); 
    if(sserveur < 0)  
    { 
       perror("Probleme de creation de socket\n ");  
       exit(-1);    
    } 
    printf("Socket cree avec succes!  \n"); 
   

   longueurAdresse = sizeof(struct   sockaddr_in); 
   memset(&sadr, 0x00, longueurAdresse); 
   sadr.sin_family = AF_INET; 
   sadr.sin_addr.s_addr =  htonl(INADDR_ANY);    
   sadr.sin_port = htons(port_si1);    

   if((bind(sserveur,(struct  sockaddr *)&sadr, longueurAdresse)) <  0) 
   { 
       perror("bind"); 
       exit(-2); 
   } 
   printf("Socket attachee avec succes!\n"); 


   longueurAdresse = sizeof(cadr); 
   memset(&cadr, 0x00, longueurAdresse); 
   memset(mrec1, 0x00, max_size*sizeof(char));  
   while(1){
   countr = recvfrom(sserveur, mrec1,  sizeof(mrec1), 0, (struct  sockaddr *)&cadr, &longueurAdresse); 
    switch(countr) 
   { 
       case  -1  :
          perror("Erreur recvfrom"); 
          close(sserveur); 
          exit(-3); 
       case  0 : 
          fprintf(stderr, "Aucune donnée n a été reçue!\n\n"); 
          close(sserveur); 
          return   0; 
       default :  
             printf(" %s Recu avec succes \n\n", mrec1); 
   } 
//traitement fichier
    fichier = fopen("factures.txt", "r");
    if (fichier != NULL)
     {  
        int somme=0;       
        while (fgets(chaine, max_size, fichier) != NULL) 
        {
            strcpy(s, chaine);
            char ch = atoi(strtok(s,":"));
            if (ch==atoi(mrec1)){
                find = 1;
                char * cid= strtok ( chaine, ":");
                char * mnt = strtok ( NULL, ":");
                char * f = strtok ( NULL, ":");
                char * fid = strtok (f, ",");
                somme += atoi(mnt);
                sprintf(st,"%s\t\t%s\t\t%s\n",fid,mnt,cid);
                strcat(menv1,st);

            }
            
        }
                strcat(menv1,"\n");
                char ft[max_size]="";
                char forma[max_size]="";
                sprintf(forma,"**************************\n");
                sprintf(ft,"* La Somme:%d          *\n",somme);
                strcat(forma,ft);
                strcat(forma,"**************************\n");
                strcat(menv1,forma); 
        if (find==0){
            printf("Facture introuvable! \n");
         }
     fclose(fichier);
    }
    else
    {
        printf("Impossible d'ouvrir le fichier factures.txt \n");
    } 

 counts = sendto(sserveur, menv1, strlen(menv1), 0,(struct sockaddr *)&cadr,longueurAdresse); 
   switch(counts) 
   { 
       case -1:
          perror("non env"); 
          close(sserveur); 
          exit(-3); 
       case 0: 
          fprintf(stderr, "Aucune  donnee  n a  ete  envoyee  !\n\n"); 
          close(sserveur); 
          return 0; 
       default : 
          if(counts != strlen(menv1)) 
             fprintf(stderr, "Erreur  dans  l envoie   des  donnees  !\n\n"); 
          else 
             printf("%s\nEnvoye avec succes au Serveur\n",menv1); 
   } 
    strcpy(menv1,"");
   }
   close(sserveur); 

   return   0; 
} 