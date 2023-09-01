#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT "11111"

void error(char *msg);
int sendmymsg(int sockfd , int number);
int define_socket(char * port , char * ip);
void send_i_msg(int sockfd ,int n);
int main(int argc, char *argv[])
{
    int sockfd;
    char *ip ;
    char *port;
    /* Opening the socket */
    if(argc == 2)
     { port = argv[2];
      ip = argv[1];}
    else{
      port = PORT;
      ip = "127.0.0.1";
    }
    sockfd = define_socket(port , ip);
    send_i_msg(sockfd ,20);  
    close(sockfd);
    return 0;
}
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int define_socket(char * port , char * ip){
  struct sockaddr_in server_addr;
  int sockfd;
  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) <0)
      error("error opening sockfet");

    /* Defining Socket */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(ip);

    /* Connect to server */
    if (connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
        error("ERROR connecting\n");
    printf("Connection established\n");
    return sockfd;
}
void send_i_msg(int sockfd , int n){
  for(int i=0 ; i<n ; i++){
      sendmymsg(sockfd,i+1);
  }
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