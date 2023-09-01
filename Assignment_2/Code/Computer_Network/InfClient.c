#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 256
void error(char *msg)
{
    perror(msg);
    exit(1);
}
int sendmymsg(int sockfd , int number){
  int n;
  long res;
   
  //printf("What client is sending %d \n" , number);
	if((n = send(sockfd,&number,sizeof(int) ,0)  )<0)
	    error("ERROR writing to socket\n");

	
	/* Read reply */
	if((n = recv(sockfd,&res,sizeof(res),0) ) <0)
    error("ERROR reading from socket\n");

	printf("Server replied: %lu\n",res);
  return 0;
     
}
int main(int argc, char *argv[])
{
    int sockfd , n;
    struct sockaddr_in server_addr;
    if (argc < 3)   
        error("Insufficient arguments");
    /* Opening the socket */
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) <0)
      error("error opening sockfet");

    /* Defining Socket */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    /* Connect to server */
    if (connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
        error("ERROR connecting\n");
    printf("Connection established\n");
    
    for(int i=1 ; i>0 ; i++){
        int number;
        printf("Enter no. ");
        scanf("%d" , &number);
        sendmymsg(sockfd,number);
    }
    
    close(sockfd);
    return 0;
}
