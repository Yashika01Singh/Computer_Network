
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include  <signal.h>
#include <pthread.h>
#define PORT  "11111"
#define MAX_CONNECTIONS 10
pthread_mutex_t lock;
FILE *fd;
int thr=0;
pthread_t t[10];
void error (char *msg);
long factorial(int num);
void * client_handler(void *a);
int make_socket(char * portno );
void handler(int sig);
struct data{
    int sockfd;
    struct sockaddr_in client;

};
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
       
        if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<=0 )
          error("ERROR on accept");
        struct data *parameter = malloc (sizeof(struct data));
        parameter->sockfd = newsockfd;
        parameter->client = cli_addr;
        printf("This is the sockf before trhead %d\n",newsockfd);
        //client_handler(parameter);
        
        if( pthread_create(&t[thr] , NULL , client_handler , parameter) <0)
            perror("thread not created");
            

       
        
        
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
    pthread_mutex_destroy(&lock);
    fclose(fd);
    exit(0);
}

long factorial(int num){
    long res= 1;
    for(int i=1 ; i<=num ; i++)
        res*=i;
    return res;
}
void * client_handler(void *a){
thr++;
struct data *parameter = (struct data *) a;
fd = fopen("Requests.txt" , "a");

int newsockfd = parameter->sockfd;
struct sockaddr_in cli_addr = parameter->client;
free (a);
printf("This is the sockf after trhead %d\n",newsockfd);
while(1){
    int number,n;
    long answer;
    if((n=recv(newsockfd,&number,sizeof(number),0) ) <=0 ){
       
        if(n==0) {
            
             return NULL;
        } else {
              perror("recv");
        }
        exit(EXIT_FAILURE);

    }
    
    
    answer=factorial(number);
    //pthread_mutex_lock(&lock);
    fprintf(fd , "%s %d from ip %s port %d %s %lu  \n" ,"Recieved ",number, inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port)," Sending",answer);
    fflush(fd);
    //pthread_mutex_unlock(&lock);
    printf("Here is the message: %d\n",number);
    
     send(newsockfd,&answer , sizeof(answer) , 0);
     
    }
    close(newsockfd);
    return NULL;
    
}

int make_socket(char * portno ){
    struct sockaddr_in serv_addr;
    int sockfd;
    if( (sockfd = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP)) <0)
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