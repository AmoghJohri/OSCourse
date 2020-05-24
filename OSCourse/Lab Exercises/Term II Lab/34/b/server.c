#include <sys/types.h>          
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h>

void* deal_with_client(void* nsd)
{
    char buffer[1024];
    int nsd_ = *(int*)nsd;
    printf("NSD: %d\n", nsd_);
    while(1){
        memset(buffer, 0, sizeof(buffer));
        recv(nsd_, buffer, sizeof(buffer), 0);
        write(1, buffer, sizeof(buffer));
        printf("Printed by thread: %ld\n", pthread_self());
    }
}

int main()
{
    char buffer[1024] = {0};

    printf("This is the server program!\n");
    int sd = socket(AF_INET, SOCK_STREAM, 0); // RAW Socket Descriptor
    if(sd == -1)
    {
        printf("Failed to initialize a socket!\n");
        exit(-1);
    }
    struct in_addr inadr;
    struct sockaddr_in serv, cli;
    int addrlen = sizeof(cli);
    serv.sin_family = AF_INET;
    inadr.s_addr = INADDR_ANY;
    serv.sin_addr = (inadr);
    serv.sin_port = htons(5001); // setting the port no. as 5000
    printf("The port is: %d\n", serv.sin_port);

    int val = bind(sd, (void*)&serv, sizeof(serv));
    if(val == -1)
    {
        if(errno == EADDRINUSE)
        {
            printf("Given address is already in use!\n");
        }
        printf("Bind failed!\n");
        printf("Process id is: %d\n", getpid());
        exit(-1);
    }
    val = listen(sd, 2);
    if(val  == -1)
    {
        printf("Listen failed\n");
        exit(-1);
    }
    printf("Server is up and running...\n");
    unsigned long thread_id;
    int nsd = -1;
    while(1)
    {
        nsd = accept(sd, (void*)&cli, (socklen_t*)&addrlen);
        printf("Connected succefully!\n");
        printf("Connected to client: %s\n",inet_ntoa(cli.sin_addr));
        thread_id += 1;
        pthread_create( &thread_id , NULL ,  deal_with_client , (void*) &nsd);
    }
    return 0;
}  