
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include  <signal.h>

#define PORT  "11111"

FILE *fd;
void error (char *msg);
long factorial(int num);
int client_handler(int newsockfd , struct sockaddr_in cli_addr ,FILE *fd );
int make_socket(char * portno );
void handler(int sig);

int main(int argc, char *argv[])
{
    int newsockfd,sockfd;
    struct sockaddr_in  cli_addr;
    signal(SIGINT , handler);

    if (argc == 1)
        sockfd = make_socket(PORT);       
    else
        sockfd = make_socket(argv[1]);
    fd = fopen("Requests.txt" , "w");
    int len  = sizeof(cli_addr); 
    while(1){
        //infinitely listening for connections
        
        if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<0 )
            error("ERROR on accept");
        client_handler(newsockfd, cli_addr , fd);
        close(newsockfd);
    }
    
    fclose(fd);  
    return 0; 
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void handler(int sig){
    printf("\nServer Quitting\n");
    fclose(fd);
    exit(0);
}

long factorial(int num){
    long res= 1;
    for(int i=1 ; i<=num ; i++)
        res*=i;
    return res;
}

int client_handler(int newsockfd , struct sockaddr_in cli_addr ,FILE *fd ){

while(1){
    int number,n;
    long answer;
    if((n=recv(newsockfd,&number,sizeof(number),0) ) <=0 ){
       
        if(n==0) {
            
             return 0;
        } else {
              perror("recv");
        }
        exit(EXIT_FAILURE);

    }
    
    
    answer=factorial(number);
    fprintf(fd , "%s %d from ip %s port %d %s %lu  \n" ,"Recieved ",number, inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port)," Sending",answer);
    fflush(fd);
    printf("Here is the message: %d\n",number);
    
     send(newsockfd,&answer , sizeof(answer) , 0);
     
    }
    
}

int make_socket(char * portno ){
    struct sockaddr_in serv_addr;
    int sockfd;
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
        error("ERROR opening socket");     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(atoi(portno));
      if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
      listen(sockfd,20);
      return sockfd;
}