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
#include <signal.h>

int check(char* read_buffer)
{
    if((int)read_buffer[0] == 49 && (int)read_buffer[1] == 0)
    {
        return -1;
    }
   return 0;
}


int main()
{
    printf("This is the client program!\n");
    int sd = socket(AF_INET, SOCK_STREAM, 0); // RAW Socket Descriptor
    
    if(sd == -1)
    {
        printf("Error: Failed To Initialize a Socket!\n");
        exit(-1);
    }

    struct in_addr inadr;
    struct sockaddr_in serv, cli;
    serv.sin_family = AF_INET;
    inadr.s_addr = INADDR_ANY;
    serv.sin_addr = (inadr);
    serv.sin_port = htons(5000); // setting the port no. as 5000
    printf("The port is: %d\n", serv.sin_port);

    int val = connect(sd, (void*)&serv, sizeof(serv));
    if(val == -1)
    {
        printf("Erro: Failed To Connect!\n");
        exit(-1);
    }

    printf("Client is connected to the server...\n");
    int i = 0;
    int id = 0;
    while(1)
    {
        char write_buffer[1024] = {0};
        char read_buffer[1024] = {0};
        if(i != 3) recv(sd, read_buffer, sizeof(read_buffer), 0);
        if(i == 0) // iteration 1 - login
        {
            write(1, read_buffer, sizeof(read_buffer));
            read(0, write_buffer, sizeof(write_buffer));
            send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            id = atoi(write_buffer);
            i = i + 1;
        }
        else if(i == 1)
        {
            if(strcmp(read_buffer, "1") == 0)
            {
                printf("Incorrect Id!\n");
                printf("Closing the connection...\n");
                exit(-1);
            }
            else if(strcmp(read_buffer, "2") == 0)
            {
                printf("Account Already Open!\n");
                printf("Closing the connection...\n");
                exit(-1);
            }
            else
            {
                write(1, read_buffer, sizeof(read_buffer));
                read(0, write_buffer, sizeof(write_buffer));
                send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                i = i + 1;
            }
        }
        else if(i == 2)
        {
            if(strcmp(read_buffer, "1") == 0)
            {
                printf("Incorrect Password!\n");
                printf("Closing the connection...\n");
                exit(-1);
            }
            else if(strcmp(read_buffer, "2") == 0)
            {
                printf("Account Already Open!\n");
                printf("Closing the connection...\n");
                exit(-1);
            }
            else
            {
                write(1, read_buffer, sizeof(read_buffer));
                i = i + 1;
            }
        }
        else // main loop
        {
            if(id != 1)
            {
                while(1)
                {
                    memset(read_buffer, 0, sizeof(read_buffer));
                    memset(write_buffer, 0, sizeof(write_buffer));

                    recv(sd, read_buffer, sizeof(read_buffer), 0);
                    if(check(read_buffer) == -1) 
                    {
                        write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                        continue;
                    }
                    write(1, read_buffer, sizeof(read_buffer));
                    read(0, write_buffer, sizeof(write_buffer));
                    send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    if((int)write_buffer[0] == 54 && (int)write_buffer[1] == 10)
                    {
                        printf("Closing the connection...\n");
                        exit(-1);
                    }
                    else if((int)write_buffer[0] == 51 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) 
                        {
                            write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                            continue;
                        }
                        write(1, "Your balance is: ", strlen("Your balance is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));
                    }
                    else if((int)write_buffer[0] == 53 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) 
                        {
                            write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                            continue;
                        }
                        write(1, "++++++++++++++\n", strlen("++++++++++++++\n"));
                        write(1, "Your Id is: ", strlen("Your Id is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);  
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, "Your Password is: ", strlen("Your Password is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, "Your Balance is: ", strlen("Your Balance is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));
                        write(1, "++++++++++++++\n", strlen("++++++++++++++\n"));
                    }
                    else if((int)write_buffer[0] == 49 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) 
                        {
                            write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                            continue;
                        }
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                    else if((int)write_buffer[0] == 50 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) 
                        {
                            write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                            continue;
                        }
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) write(1, "Insufficient Balance\n", strlen("Insufficient Balance\n"));
                    }
                }
            }
            else
            {
                while(1)
                {
                    memset(read_buffer, 0, sizeof(read_buffer));
                    memset(write_buffer, 0, sizeof(write_buffer));
                    recv(sd, read_buffer, sizeof(read_buffer), 0);
                    if(check(read_buffer) == -1) 
                    {
                        write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                        continue;
                    }
                    write(1, read_buffer, sizeof(read_buffer));
                    read(0, write_buffer, sizeof(write_buffer));
                    send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    if((int)write_buffer[0] == 52 && (int)write_buffer[1] == 10)
                    {
                        printf("Closing the connection...\n");
                        exit(-1);
                    }
                    else if((int)write_buffer[0] == 51 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) 
                        {
                            write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                            continue;
                        }
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1)
                        {
                            write(1, "Account ID Invalid!\n", strlen("Account ID Invalid!\n"));
                            continue;
                        }
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, "++++++++++++++\n", strlen("++++++++++++++\n"));
                        write(1, "Your Id is: ", strlen("Your Id is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);  
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, "Your Password is: ", strlen("Your Password is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, "Your Balance is: ", strlen("Your Balance is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));
                        write(1, "++++++++++++++\n", strlen("++++++++++++++\n"));
                    }
                    else if((int)write_buffer[0] == 50 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1) 
                        {
                            write(1, "Invalid Input, Try Again!\n", strlen("Invalid Input, Try Again!\n"));
                            continue;
                        }
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1)
                        {
                            write(1, "Account ID Invalid!\n", strlen("Account ID Invalid!\n"));
                            continue;
                        }
                        else
                        {
                            write(1, "Account Deleted!\n", strlen("Account Deleted!\n"));
                            continue; 
                        }
                    }
                    else if((int)write_buffer[0] == 49 && (int)write_buffer[1] == 10)
                    {
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(read_buffer[0] == (int)'-')
                        {
                            write(1, "Invalid Number Of Accounts!\n", strlen("Invalid Number Of Accounts!\n"));
                            continue;
                        }
                        int num_of_accounts = atoi(read_buffer);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, read_buffer, sizeof(read_buffer));
                        int tag = 0;
                        for(int i = 0; i < num_of_accounts; i++)
                        {
                            memset(read_buffer, 0, sizeof(read_buffer));
                            memset(write_buffer, 0, sizeof(write_buffer));
                            read(0, write_buffer, sizeof(write_buffer));
                            send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                            recv(sd, read_buffer, sizeof(read_buffer), 0);
                            if(check(read_buffer) == -1)
                            {
                                write(1, "Invalid Account Id\n", strlen("Invalid Account Id\n"));
                                tag = 1;
                                break;
                            }
                        }
                        if(tag == 1)
                            continue;
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                }
            }
                
        }
        
    }
    return 0;
}