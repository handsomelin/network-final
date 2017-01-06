/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

#include<arpa/inet.h> //inet_addr



#define MAX_THREADS 10

int threadcounter = 0, create = 0;
void *welcome(void *param);
int findid(char *id_name);
void check();


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

     int sockfd, /*newsockfd,*/ portno;
     socklen_t clilen[MAX_THREADS];									////////////////////////////////
     int newsockfd[MAX_THREADS];
	int nowthread = 0;
	 struct sockaddr_in serv_addr, cli_addr[MAX_THREADS];
	char client_name[10][100]={};
	int online_number = 0;
	int nowsocket[10] = {0};
	int newi = 0;
	int money[10];
     	
struct arg
{
	int threadn;
	struct sockaddr_in addr;
};
     
int main(int argc, char *argv[])
{
     for(int i=0;i<10;i++)
     {
     	money[i] = 10000;
     }
     pthread_t accept_thread[MAX_THREADS];
     
    			////////////////////////////////
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,10);
     printf("Wait For Connection !\n");
     int i = 0;
     for(i = 0;i<10;i++)
     {
     	clilen[i] = sizeof(cli_addr[i]);
     }
	
	struct arg thread_arg[10];
     for(i = 0;i<10;i++)
     {
	printf("%d\n", i);
	void *pointer = &thread_arg[i];
	thread_arg[i].threadn = i;//now;
	newsockfd[i] = accept(sockfd, (struct sockaddr *) &cli_addr[ i ], &clilen[ i ]);
     	if (newsockfd[i] < 0) 
        {
        	error("ERROR on accept");
		continue;
	}
        else
        {
		thread_arg[i].addr = cli_addr[i];
		char *client_ip = inet_ntoa(cli_addr[i].sin_addr);
        	printf("somebody connect me ip is %s , port is %d\n", client_ip , cli_addr[i].sin_port);
        	printf("on:%d\n ", online_number);
		create = pthread_create(&accept_thread[i], NULL, welcome, (void *) pointer);
		nowsocket[i] = 1;
		//pthread_join();
	}
		threadcounter++;
		
     }
     //close(newsockfd);
     close(sockfd);
     return 0; 
}
void *welcome(void *param)
{
	int n;
	char buffer[1024];
	char buffer2[1024];
	char reply[1024];
	char name[100];
	char number[10];
	char port[10];
	char my_money[10];
	int login_id;
	
	struct arg* thread_arg2 = (struct arg*) param;

	bzero(buffer,1024);	
	bzero(buffer2,1024);
	strcpy(buffer, "Hello! Please Register Or Log In : \n");
	n = write(newsockfd[ thread_arg2->threadn ], buffer,strlen(buffer));
	//if (n < 0) {error("ERROR writing to socket");}


	while(1==1)
	{	
		bzero(buffer,1024);
		bzero(buffer2,1024);
    	n = read(newsockfd[ thread_arg2->threadn ],buffer,1024);
		if (n < 0) 
		{
			error("ERROR reading from socket");
		}
		printf("Client Ask : %s\n", buffer);
		strcpy(buffer2, buffer);
	 	char *text1 = strtok(buffer, "#");
	    char *text2 = strtok(NULL, "#");
	    char *text3 = strtok(NULL, "#");
		if(strcmp("REGISTER", text1)==0)													//////////////////REGISTER
		{
		    FILE *fPtr = fopen("register.txt", "r+");
		   	int same = 0;   	
			while (fscanf(fPtr, "%s", name) != EOF)
		    {
		    	if(strcmp(name, text2)==0)
		   		{
		   			n = write(newsockfd[ thread_arg2->threadn ],"210 FAIL\n",8);
		   			if (n < 0)
					{
						error("ERROR writing to socket");		
					}
					same = 1;
					break;
				}
				bzero(name,100);
			}
			if(same==0)
			{
				fprintf(fPtr, "%s ", text2);
				n = write(newsockfd[ thread_arg2->threadn ],"100 OK\n",6);
	    		if (n < 0) error("ERROR writing to socket");
			}
			fclose(fPtr);
		}
		else																	/////////////////////LOG IN
		{
			FILE *fPtr = fopen("register.txt", "r+");
			FILE *fPtr2 = fopen("login.txt", "a+");
	    	int same = 0;
	    	int register_id = 0;
	    	int payee_id;   	
				while (fscanf(fPtr, "%s", name) != EOF)
	    		{
	    			
	    			if( strcmp(name, text1) == 0 )									/////////////////////////LOG IN OK
	    			{
	    				same = 1;
	    				if( text3 != NULL )
	    				{
	    					payee_id = findid(text3);
	    					money[register_id] = money[register_id] - atoi(text2);
	    					money[payee_id] = money[payee_id] + atoi(text2);
	    					n = write(newsockfd[ thread_arg2->threadn ],"SUCCESS\n",13);
	    					if (n < 0) error("ERROR writing to socket");
	    					break;
	    				}
	    				fprintf(fPtr2, "%s ", buffer2);
	    				
						//strcat( client_name[ online_number ] , name );
						strcpy( client_name[ thread_arg2->threadn ] , buffer2 ); 
						online_number++;
						login_id = register_id;
						bzero(reply,1024);
						bzero(port,10);
						bzero(my_money,10);
						strcat(reply, "AccountBalance is ");
						sprintf(my_money, "%d\n", money[login_id] );	
						strcat(reply, my_money);
						strcat(reply, "Online Number Is ");
						sprintf(number, "%d\n", online_number);
						strcat(reply, number);
						//strcat(reply, "\n");
						int o;
						for( o = 0; o< 10 ;o++)
						{
							if(nowsocket[o] == 0)
							{
								continue;
							}
							else
							{
								char *client_ip2 = inet_ntoa(cli_addr[ o ].sin_addr);
								
								strcat(reply, "userIp is ");
								
								strcat(reply, client_ip2);
								strcat(reply, " # ");
								
							strcat(reply, "user is ");
								strcat(reply, client_name[o]);
								strcat(reply, "\n");
								// sprintf(port, "%d\n", cli_addr[o].sin_port );	
								// strcat(reply, port);
							}

						}
	    				n = write(newsockfd[ thread_arg2->threadn ], reply, strlen(reply));
	    				if (n < 0)
						{
							error("ERROR writing to socket");		
						}
						
						break;
					}
					bzero(name,100);
					register_id++;
				}
			if(same==0 && strcmp( text1 ,"List")!=0 && strcmp( text1 ,"Exit")!=0)
			{
				n = write(newsockfd[ thread_arg2->threadn ],"220 AUTH_FAIL\n",13);
	    			if (n < 0) error("ERROR writing to socket");	
			}
			else if(same == 0 && strcmp( text1 ,"List")==0)
			{
				check();
				bzero(reply,1024);
				bzero(port,10);
				bzero(my_money,10);

				strcat(reply, "AccountBalance is ");
				sprintf(my_money, "%d\n", money[login_id] );	
				strcat(reply, my_money);
				strcat(reply, "Online Number Is ");
				sprintf(number, "%d\n", online_number);
				strcat(reply, number);
					//strcat(reply, "\n");
				int o;
				//char oo[10] = {};
				for( o = 0; o< 10 ;o++)
				{
					if(nowsocket[o] == 0)
						{
							continue;
						}
					else
					{
						char *client_ip3 = inet_ntoa(cli_addr[ o ].sin_addr);
						
						strcat(reply, "userIp is ");
						
						strcat(reply, client_ip3);
						strcat(reply, " # ");
						
						strcat(reply, "user is ");
						strcat(reply, client_name[o]);
						strcat(reply, "\n");
					}
					
				}
				//printf("%s", reply);
    				n = write(newsockfd[ thread_arg2->threadn ], reply, strlen(reply));
    				if (n < 0)
				{
					error("ERROR writing to socket");		
				}
			}
			else if(same == 0 && strcmp( text1 ,"Exit")==0)
			{
				int o;
				n = write(newsockfd[ thread_arg2->threadn ], "Bye", 3);
    				if (n < 0)
				{
					error("ERROR writing to socket");		
				}
				for(o = thread_arg2->threadn ; o < online_number ;o++)
				{
					printf( "%s %s\n", client_name[o] , client_name[o+1]);
					strcpy( client_name[o] , client_name[o+1] );
					cli_addr[o] = cli_addr[o+1];
					clilen[o] = clilen[o+1];

					if(o==online_number-1)
					{
						bzero( client_name[o], strlen(client_name[o]) );
						//cli_addr[o] = cli_addr[o+1];
						clilen[o] = 0;
					}
				}
				online_number--;
				nowsocket[ thread_arg2->threadn ] = 0;
				threadcounter--;
				break;
			}
			fclose(fPtr);
			fclose(fPtr2);
		}
	}
	close(newsockfd[ thread_arg2->threadn ]);
}
int findid(char *id_name)
{
	char name[100];
	bzero(name,100);
	FILE *name_Ptr = fopen("register.txt", "r+");
	int request_id = 0;   	
	while (fscanf(name_Ptr, "%s", name) != EOF)
	{   			
	   	if( strcmp(name, id_name) == 0 )									/////////////////////////LOG IN OK
	   	{
	   		return request_id;
	   	}
	   	bzero(name,100);
		request_id++;
	}
	fclose(name_Ptr);
	return -1;
}
void check()
{
	for(int i=0;i<10;i++)
	{
		printf("%d\n", money[i]);
	}
}
