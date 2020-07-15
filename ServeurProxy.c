#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<netinet/in.h>    
#include<netdb.h>  
    

#define max_size 4096
#define ip_address "127.0.0.1"
#define port_si1 2058
#define port_si2 3030 
#define port 5000

void server1call(int id,char *forma){
    int  sclient;                 
    struct  sockaddr_in sadr; 
    unsigned int  sadrlen;    
    char menv[max_size]="";
    char mrec[max_size]="";  
    char* idc,m,idf;        
    unsigned short ServPort;      
    char adresseserveurip[32];    
    int  counts,countr;                   
	
	 sclient = socket(AF_INET,SOCK_DGRAM,0); 
    if(sclient < 0)  
    { 
       perror("Probleme de creation de socket\n ");    
       exit(-1);    
    } 
    printf("Socket cree avec succes!  \n"); 
   
   
    sadr.sin_family=AF_INET;
    sadr.sin_port=htons(port_si1);
    sadr.sin_addr.s_addr=inet_addr(ip_address);
    memset(&sadr.sin_zero,0,8);

  
   /*memset(menv,0x00, max_size*sizeof(char)); */
   sprintf(menv, "%d", id);

   counts = sendto(sclient,menv,strlen(menv),0,(struct sockaddr *)&sadr,sizeof(sadr));
   switch(counts) 
   { 
       case -1:
          perror("Erreur de sendto\n\n"); 
          close(sclient); 
          exit(-3); 
       case 0: 
          fprintf(stderr, "Aucune  donnee  n'a été envoyée  !\n\n"); 
          close(sclient); 
          return ; 
       default : 
          if(counts != strlen(menv)) 
             fprintf(stderr, "Erreur  dans  l'envoie   des  données  !\n\n"); 
          else 
             printf("ID envoyé avec succes\n"); 
   } 
	countr=recvfrom(sclient, mrec,  sizeof(mrec), 0, (struct  sockaddr *)&sadr, &sadrlen); 
	switch(countr) 
  	 { 
       case -1:
          perror("Reponse non recu!\n"); 
          close(sclient); 
          exit(-3); 
       case 0: 
          fprintf(stderr, "Aucune  donnée  n'a  été  recu  !\n"); 
          close(sclient); 
          return ; 
       default : 
          if(countr != strlen(mrec)) 
             fprintf(stderr, "Erreur  dans  la reception   des  données  !\n"); 
          else 
             printf("Recu avec succes de la part de SI1 \n"); 
   } 
    char ft[max_size]="";
    memset(forma,0x00, max_size*sizeof(char));
    sprintf(forma,"*************************************\n");
    sprintf(ft,   "* Les factures du client %d dans SI1*\n",id);
    strcat(forma,ft);
    strcat(forma,"*************************************\n");
    strcat(forma,"IDfacture\tMontant\t\tIDclient\n");
    strcat(forma,mrec);
    close(sclient);
    
    return ;
}

void server2call(int id, char *forma){
    int sclient;
    struct sockaddr_in sadr={0};
    char menv[max_size];
    char mrec[max_size]="";
    sclient=socket(AF_INET,SOCK_STREAM,0);
    sadr.sin_family=AF_INET;
    sadr.sin_port=htons(port_si2);
    sadr.sin_addr.s_addr=inet_addr(ip_address);
    while(connect(sclient,(struct sockaddr *)&sadr,sizeof(sadr))==-1);
    sprintf(menv, "%d", id);
    /*send(sclient,menv,strlen(menv)+1,0);*/
    write(sclient,menv,strlen(menv)+1);
    /*recv(sclient,mrec,strlen(mrec)+1,0);*/
    read(sclient,mrec,max_size-1);
    char ft[max_size]="";
    memset(forma,0x00, max_size*sizeof(char));
    sprintf(forma,"*************************************\n");
    sprintf(ft,   "* Les factures du client %d dans SI2*\n",id);
    strcat(forma,ft);
    strcat(forma,"*************************************\n");
    strcat(forma,"IDfacture\tMontant\t\tIDclient\n");
    strcat(forma,mrec);
   shutdown(sclient, SHUT_RDWR);
    close(sclient);
    return ;
}

void server12call(int id,char *rep){
    char req1[max_size];
    char req2[max_size];
    server1call(id,req1);
    server2call(id,req2);
    strcat(rep,req1);
    strcat(rep,"\n");
    strcat(rep,req2);

    return ;
}

int main(int argc, char* argv[]){
    
    int sserveur, sservice;
    unsigned int sockadlen;
    struct sockaddr_in saddr={0}, caddr={0};
    char reponse[max_size]; char msgrec[max_size];
    char code,id;
    char* r1;

    sserveur=socket(AF_INET,SOCK_STREAM,0);
    if(sserveur < 0)  
    { 
       perror("Probleme de creation de socket\n ");  
       exit(-1);    
    } 
    printf("Socket cree avec succes!  \n"); 
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);
    saddr.sin_addr.s_addr=inet_addr(ip_address);

    if((bind(sserveur,(struct  sockaddr *)&saddr, sizeof(saddr))) <  0) 
   { 
       perror("bind"); 
       exit(-2); 
   } 
   printf("Socket attachee avec succes!\n"); 
    listen(sserveur,5);
    printf("Serveur en ecoute\n");
    while(1){
        sservice=accept(sserveur,(struct sockaddr*)&caddr,&sockadlen);
        /*recv(sservice,msgrec,strlen(msgrec)+1,0);*/
        memset(msgrec,'\0',max_size); 
        read(sservice,msgrec,max_size);
        printf("%s",msgrec);
        code = atoi(strtok(msgrec,";")); 
        id=atoi(strtok(NULL,";")); 
        switch (code)
        {
            case 1:
                server1call(id,reponse);
                printf("Les factures du client %s envoyees",reponse);
                break;
            case 2:
                server2call(id,reponse);
                printf("Les factures du client %s envoyees",reponse);
                break;
            case 3:
                server12call(id,reponse);
                printf("Les factures du client %s envoyees",reponse);
                break;
        }
        /*send(sservice,reponse,strlen(reponse)+1,0);*/
          write(sservice,&reponse,strlen(reponse)+1);

        
         shutdown(sservice,SHUT_RDWR);
         close(sservice);
        
    }
    close(sserveur);
    return 0;
}