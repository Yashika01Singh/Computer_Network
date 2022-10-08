
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

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
    listen(sockfd,20);
    
     
    int number;
    long answer;
    int len  = sizeof(cli_addr);
    if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<0 )
          error("ERROR on accept");
    
    while(1){
     
     if((n = recv(newsockfd,&number,sizeof(number),0) ) <0 )
        error("ERROR reading from socket");
    FILE *fd = fopen("Requests.txt" , "a");
    answer=factorial(number);
    fprintf(fd , "%s %d %s %lu \n" ,"Recieved ",number," Sending",answer);
    fclose(fd);
    printf("Here is the message: %d\n",number);
    
     n = send(newsockfd,&answer , sizeof(answer) , 0);
     
    }
    
    close(newsockfd);
     return 0; 
}