#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

#include<arpa/inet.h>

#define MAX_THREADS 10

int threadcounter = 0, create = 0;
void *listen_client(void *myport);
void *welcome(void *param);
// void *thread_listen(void *param);
void *connect_client(void *param);
pthread_t accept_thread[MAX_THREADS];
pthread_t thread_id, thread_id2;

//int findid(char *id_name);
//void check();

int req=0;
const char reg[20] = "REGISTER#";
char request[1024];
char wait_port[10];
char transform_port[10];
char transform_ip[50];
char trade_order[1024];
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void ask()
{
    printf("Enter 1 for Register, 2 for Login, 3 for Exit, 4 for List, 6 for Transform: ");
    while(scanf("%d", &req) < 1)
    {
        char wrong[100];
        scanf("%s", wrong);
        printf("Enter 1 for Register, 2 for Login, 3 for Exit, 4 for List, 6 for Transform: ");

    }
}
void ask2()
{
    printf("Enter 3 for Exit, 4 for List, 6 for Transform: ");
    while(scanf("%d", &req) < 1)
    {
        char wrong[100];
        scanf("%s", wrong);
        printf("Enter 3 for Exit, 4 for List, 6 for Transform: ");

    }
}
void Register()
{
    char name[1024];
    bzero(request,1024);
    bzero(name, 1024);
    printf("Please enter the name you want to register:");
    scanf("%s", name);
    strcpy(request, reg);
    strcat(request,name);
}
void Login()
{
    char name[1024];
    bzero(request,1024);
    bzero(name, 1024);
    bzero(wait_port, 1024);
    printf("Please enter your name:");
    scanf("%s", name);
    printf("Please enter your port:");
    scanf("%s", wait_port);
    strcat(request,name);
    strcat(request,"#");
    strcat(request,wait_port);
}
void Exit()
{
    bzero(request,1024);
    strcpy(request, "Exit");
}
void List()
{
    bzero(request,1024);
    strcpy(request, "List");
}
void Trade()
{
    bzero(request,1024);
    strcpy(request, trade_order);
}
void Transform()
{
    
    char name[1024], myname[1024], money[50];
    bzero(request,1024);
    bzero(name, 1024);
    bzero(transform_port, 10);
    bzero(myname, 1024);
    bzero(transform_ip, 50);
    bzero(money, 50);

    printf("Please enter other's name:");
    scanf("%s", name);
    printf("Please enter their ip:");
    scanf("%s", transform_ip);
    printf("Please enter their port:");
    scanf("%s", transform_port);
    printf("How much you want to trade?");
    scanf("%s", money);
    printf("Please enter my name:");
    scanf("%s", myname);
    
    strcat(request,myname);
    strcat(request,"#");
    strcat(request,money);
    strcat(request,"#");
    strcat(request,name);
}

int login_ok = 0, only_one = 0, receive_money = 0;

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    

    char buffer[1024];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    //bzero(buffer,1024);
    n = read(sockfd,buffer,1023);
    if (n < 0) 
        error("ERROR reading from socket");
    printf("%s\n",buffer);
    while(strcmp(buffer,"bye") != 0)
    {
        //bzero(buffer,1024);
        //printf("Please enter the message: ");
        if(only_one == 1)
        {
            only_one = 0;
            pthread_create(&thread_id2, NULL, listen_client, (void *) wait_port);
            //listen_client(wait_port);
        }
        if(login_ok == 1)
        {
            ask2();
        }
        else
        {
            ask();
            if(req == 1)
            {
                Register();
            }
            else if (req == 2)
            {
                Login();
                login_ok = 1;
                only_one = 1;
            }
        }
            
            if (req == 3)
            {
                Exit();
            }
            else if (req == 4)
            {
                List();
            }
            else if (req == 6)
            {
                Transform();
                char *noneed_pointer2;
                pthread_create(&thread_id, NULL, connect_client, (void *) noneed_pointer2);
            }
            else if ((req == 1)||(req == 2)||(req == 5487))
            {
                if(req == 5487)
                {
                    Trade();
                    receive_money = 0;
                }
            }
            else
            {
                continue;
            }
            //fgets(buffer,1023,stdin);
            if(req != 6)
            {   
                n = write(sockfd,request,strlen(request));
                if (n < 0) 
                    error("ERROR writing to socket");
                bzero(buffer,1024);
                n = read(sockfd,buffer,1023);
                if (n < 0) 
                    error("ERROR reading from socket");
                printf("%s\n",buffer);
            }
            
    }
    close(sockfd);
    return 0;
}       
struct arg
{
    int threadn;
    struct sockaddr_in addr;
};

    int mysockfd;
    socklen_t clilen[MAX_THREADS]; 
    int newsockfd[MAX_THREADS];
    struct sockaddr_in cli_addr[MAX_THREADS];

void *listen_client(void *myport)
{
    int /*mysockfd, newsockfd,*/ portno;
                                    ////////////////////////////////
    //int newsockfd[MAX_THREADS];
    int nowthread = 0;
    struct sockaddr_in serv_addr;
    char client_name[10][100]={};
    int newi = 0;
    int money[10];
    
                ////////////////////////////////
    mysockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (mysockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(wait_port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(mysockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    //struct arg noneed;
    //void *noneed_pointer = &noneed;
    //pthread_create(&accept_thread[9], NULL, thread_listen, (void *) noneed_pointer);//carefully use the last pthread
    listen(mysockfd,10);
    printf("\nWait For Connection !\n");
    for(int i = 0;i<10;i++)
    {
       clilen[i] = sizeof(cli_addr[i]);
    }
    struct arg thread_arg[10];
    for(int i = 0;i<10;i++)//carefully cut the last pthread
    {
        //printf("%d\n", i);
        void *pointer = &thread_arg[i];
        thread_arg[i].threadn = i;//now;
        newsockfd[i] = accept(mysockfd, (struct sockaddr *) &cli_addr[ i ], &clilen[ i ]);
        if (newsockfd[i] < 0) 
        {
            error("ERROR on accept");
            continue;
        }
        else
        {
            thread_arg[i].addr = cli_addr[i];
            char *client_ip = inet_ntoa(cli_addr[i].sin_addr);
            printf("\nsomebody connect me ip is %s , port is %d\n", client_ip , cli_addr[i].sin_port);
            create = pthread_create(&accept_thread[i], NULL, welcome, (void *) pointer);
            //pthread_join();
        }
        threadcounter++;
    }
    close(mysockfd);
     
}
void *welcome(void *param)
{
    int n;
    char buffer[1024];
    
    struct arg* thread_arg2 = (struct arg*) param;
    
    while(1==1)
    {   
        bzero(buffer,1024);
        bzero(trade_order,1024);
        n = read(newsockfd[ thread_arg2->threadn ],buffer,1024);
        if (n < 0) 
        {
            error("ERROR reading from socket");
        }
        strcpy(trade_order, buffer);
        char *text1 = strtok(buffer, "#");
        char *text2 = strtok(NULL, "#");
        char *text3 = strtok(NULL, "#");

        printf("%s Want To Give Me About : $%s\n", text1, text2);
        

        n = write(newsockfd[ thread_arg2->threadn ],"\nI've Got It!\nThank you very much!\nDo you want to back to main menu?",72);
        if (n < 0)
        {
            error("ERROR writing to socket");       
        }
        break;
    }
    receive_money = 1;
    printf("Please press 5487 if you want to get the money!\n");
    close(newsockfd[ thread_arg2->threadn ]);
    
}


int connect_sockfd;

void *connect_client(void *param)
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1024];
    
    portno = atoi(transform_port);
    connect_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(transform_ip);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(connect_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    n = write(connect_sockfd,request,strlen(request));
    if (n < 0) 
        error("ERROR writing to socket");
    bzero(buffer,1024);
    n = read(connect_sockfd,buffer,1023);
    if (n < 0) 
        error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(connect_sockfd);
}