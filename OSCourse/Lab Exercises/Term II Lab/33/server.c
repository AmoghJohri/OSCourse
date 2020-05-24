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
    serv.sin_port = htons(5000); // setting the port no. as 5000
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
    val = listen(sd, 1);
    if(val  == -1)
    {
        printf("Listen failed\n");
        exit(-1);
    }
    printf("Server is up and running...\n");

    int nsd = accept(sd, (void*)&cli, (socklen_t*)&addrlen);
    if(nsd > -1)
        printf("Connected succefully!\n");
    else
    {
        printf("Connection failed!\n");
        exit(-1);
    }
    if(recv(nsd, buffer, 1024, 0) <= 0)
        printf("Read failed!\n");
    printf("%s", buffer);

    char* hello_back = "Hello from server!\n";
    if(send(nsd, hello_back, strlen(hello_back), 0) <= 0)
        printf("Send failed!\n");
    else 
        printf("Hello message sent from server!\n");
    
    return 0;

}