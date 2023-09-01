
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>


void error(char *msg)
{
    perror(msg);
    exit(1);
}
long factorial(int num){

    long res= 1;
    for(int i=1 ; i<=num ; i++)
        res*=i;
    return res;
}
int client_handler(int newsockfd , struct sockaddr_in cli_addr,FILE *fd){

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

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2)
        error("Insufficient arguments");
         
     if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
        error("ERROR opening socket");
     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
    listen(sockfd,1);
    
    pid_t pid;
    int len; 
    FILE *fd = fopen("Requests.txt" , "w");
    for(;;){
    len  = sizeof(cli_addr);
    if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<0 )
          error("ERROR on accept");
    
    if(( pid=fork()) == -1){
        close(newsockfd);
        continue;
    }
    else if( pid >0){
        close(newsockfd);
        ///printf("client dropped\n");
        continue;
        
    }
    else if(pid ==0){
        //child 
        close(sockfd);
        client_handler(newsockfd, cli_addr,fd);
        printf("SOCKET CLOSED \n");
       
        break;
    }
    }
    
    return 0; 
}