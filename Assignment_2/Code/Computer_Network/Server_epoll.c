
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include  <signal.h>
#include <sys/epoll.h>
#include <pthread.h>
#define PORT  "11111"
#define MAX_CONNECTIONS 15

pthread_mutex_t lock;
struct epoll_event *events;
FILE *fd;
void error (char *msg);
long factorial(int num);
int client_handler(int newsockfd , struct sockaddr_in cli_addr ,FILE *fd );
int make_socket(char * portno );
void handler(int sig);
int next;
struct data{
    int sockfd;
    struct sockaddr_in client;

};
struct data alltheconnections[20];
struct sockaddr_in get_client(int mysockfd){
    for(int i=0; i<20; i++){
        struct data connection= alltheconnections[i];
        if(connection.sockfd == mysockfd){
            return connection.client;
        }
    }
}
int main(int argc, char *argv[])
{
    int newsockfd,sockfd;
    struct sockaddr_in  cli_addr;
    signal(SIGINT , handler);
    struct epoll_event event;
    
    if (argc == 1)
        sockfd = make_socket(PORT);       
    else
        sockfd = make_socket(argv[1]);
    fd = fopen("Requests.txt" , "w");
    int len  = sizeof(cli_addr); 
    
    int efd = epoll_create1(0);
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    epoll_ctl (efd, EPOLL_CTL_ADD, sockfd, &event);
    events = calloc (MAX_CONNECTIONS, sizeof event);
    int ret_val;
    while(1){ 
        //infinitely listening for connections
       
        ret_val = epoll_wait (efd, events, MAX_CONNECTIONS, -1);
        //now we analyse based on ret_val
        
        for(int i=0 ; i<ret_val ;i++){
            if(!(events[i].events & EPOLLIN)){
                perror("ERRORRRRR VERY BAD");
            }
            else if (sockfd == events[i].data.fd){
                //new connection
                if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<0 )
                            error("ERROR on accept");

                struct data parameter;
                parameter.sockfd = newsockfd;
                parameter.client = cli_addr;
                alltheconnections[next]=parameter;
                next++;
                //adding new event;
                event.data.fd = newsockfd;
                event.events = EPOLLIN;
                epoll_ctl (efd, EPOLL_CTL_ADD, newsockfd, &event);
            }
            else{
                cli_addr = get_client(events[i].data.fd);
                if(client_handler(events[i].data.fd, cli_addr , fd) == -1){
                    //removing the event
                    close (events[i].data.fd);

                }
            }
        }

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
    free(events);
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


    int number,n;
    long answer;
    printf("HERE SOCKFD %d\n" ,newsockfd);
    if((n=recv(newsockfd,&number,sizeof(number),0) ) <=0 ){
       
        if(n==0) {
             close(newsockfd);
             return -1;
        } else {
              perror("recv errr");
        }
        exit(EXIT_FAILURE);

    }
    
    
    answer=factorial(number);
    pthread_mutex_lock(&lock);
    fprintf(fd , "%s %d from ip %s port %d %s %lu  \n" ,"Recieved ",number, inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port)," Sending",answer);
    fflush(fd);
    printf("Here is the message: %d\n",number);
    pthread_mutex_unlock(&lock);
    send(newsockfd,&answer , sizeof(answer) , 0);
    return 0;
    
    
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