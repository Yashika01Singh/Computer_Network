
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include  <signal.h>
#include <poll.h>
#include <pthread.h>
#define PORT  "11111"
#define MAX_CONNECTIONS 15
pthread_mutex_t lock;
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
{   next=0;
    int newsockfd,sockfd;
    struct sockaddr_in  cli_addr;
    signal(SIGINT , handler);
    struct pollfd *pfds ,pollfds[MAX_CONNECTIONS];
    
    if (argc == 1)
        sockfd = make_socket(PORT);       
    else
        sockfd = make_socket(argv[1]);
    fd = fopen("Requests.txt" , "w");
    //for listen socket
    pollfds[0].fd = sockfd;
    pollfds[0].events = POLLIN;
    int len  = sizeof(cli_addr); 
    //initialising events
    for(int i=1 ; i< MAX_CONNECTIONS ; i++){
        pollfds[i].fd =0;
        pollfds[i].events = POLLIN;
    }
    
    while(1){ 
        //infinitely listening for connections
       
        int ret_val = poll(pollfds, next+1 , 5000);
        //based on ret_val analysis
        if(ret_val<=0)
            continue;
        
        for(int i=0; i<MAX_CONNECTIONS ;i++){
            if(pollfds[i].revents & POLLIN){
                //it means event occured
                if(pollfds[i].fd == sockfd){
                    //that means new connection
                    if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<0 )
                            error("ERROR on accept");
                       // printf("HERE SOCKFD %d and new %d\n" ,sockfd,newsockfd);
                          struct data parameter;
                          parameter.sockfd = newsockfd;
                          parameter.client = cli_addr;
                          alltheconnections[next]=parameter;
                          next++;
                        for(int i=0 ; i<MAX_CONNECTIONS ; i++ ){
                            if(pollfds[i].fd ==0){
                                pollfds[i].fd = newsockfd;
                                pollfds[i].events = POLLIN;
                                break;
                            }
                        }
                }
                else{
                    //existing connection wants to talk
                    cli_addr = get_client(pollfds[i].fd);
                    if(client_handler(pollfds[i].fd, cli_addr , fd) == -1){
                     
                     int save =i;
                     int j; 
                     pollfds[i].fd =0;
                     pollfds[i].events =0;
                     pollfds[i].revents =0;
                     for( j= i+1 ; j<MAX_CONNECTIONS ; j++){
                        if(pollfds[j].fd >0){
                            pollfds[i].fd=pollfds[j].fd;
                            pollfds[i].events =pollfds[j].events ;
                            pollfds[i].revents =pollfds[j].revents ;
                            pollfds[j].fd =0;
                            pollfds[j].events =0;
                            pollfds[j].revents =0;
                            i=j;
                        }

                        else
                        break; 
                     }
                     i=save;
                     
                     
                    }
                    
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
    //printf("HERE SOCKFD %d\n" ,newsockfd);
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