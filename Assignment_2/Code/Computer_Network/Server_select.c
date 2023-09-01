
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include  <signal.h>
#include <sys/select.h>
#include <fcntl.h>
#include <pthread.h>
#define PORT  "11111"

FILE *fd;
int next;
pthread_mutex_t lock;
void error (char *msg);
long factorial(int num);
int client_handler(int newsockfd , struct sockaddr_in cli_addr ,FILE *fd );
int make_socket(char * portno );
void handler(int sig);
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

    if (argc == 1)
        sockfd = make_socket(PORT);       
    else
        sockfd = make_socket(argv[1]);
    fd = fopen("Requests.txt" , "w");
    int len  = sizeof(cli_addr); 

    fd_set current_sockets , ready_sockets;
    //initialising the sockets
    FD_ZERO(&current_sockets);
    //adding sockfd
    FD_SET(sockfd , &current_sockets);

    int nret;
    while(1){
        //infinitely listening for connections
        //copying sockets as select damages the ready sockets
        ready_sockets = current_sockets;
        if((nret=select(FD_SETSIZE , &ready_sockets, NULL, NULL, NULL) )<0)
            error("ERROR on select");
        if(nret==0){
            printf("NOTHING \n");
        }
            
        //now 
        for(int i=0 ; i<FD_SETSIZE ; i++){
            if(FD_ISSET(i, &ready_sockets)){
                //socket is open for communication
                if(i==sockfd){
                        //new connection
                         if( (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &len) )<0 )
                            error("ERROR on accept");
                         //adding new sockfd to current sockets
                          struct data parameter;
                          parameter.sockfd = newsockfd;
                          parameter.client = cli_addr;
                          alltheconnections[next]=parameter;
                          next++;
                         FD_SET(newsockfd, &current_sockets);
                         
                        }
                else{
                    //existing connection wants to communicate
                    cli_addr = get_client(i);
                    if(client_handler(i, cli_addr , fd) == -1){
                        //if client wants to stop communication then removing it from current sockets
                        FD_CLR(i,&current_sockets);                        
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
    if((n=recv(newsockfd,&number,sizeof(number),0) ) <=0 ){
       
        if(n==0) {
            close(newsockfd);
             return -1;
        } else {
              perror("recv");
        }
        exit(EXIT_FAILURE);

    }
    
    
    answer=factorial(number);
    pthread_mutex_lock(&lock);
    fprintf(fd , "%s %d from ip %s port %d %s %lu  \n" ,"Recieved ",number, inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port)," Sending",answer);
    fflush(fd);
    
    pthread_mutex_unlock(&lock);
    printf("Here is the message: %d\n",number);
    
    
    send(newsockfd,&answer , sizeof(answer) , 0);
    return 0;
    
    
}

int make_socket(char * portno ){
    
    struct sockaddr_in serv_addr;
    int sockfd;
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0)
        error("ERROR opening socket");     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     /*int flags = fcntl(sockfd, F_GETFL);
     fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);*/
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(atoi(portno));
      if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
      listen(sockfd,20);
      return sockfd;
}