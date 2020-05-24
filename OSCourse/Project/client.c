#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>    
#include <netinet/in.h>      
#include <sys/socket.h>


int check(char* read_buffer) // checks whether the read_buffer contains 0 or not (if it does not contain 0, that means server is indicating an error)
{
    if((int)read_buffer[0] == 49 && (int)read_buffer[1] == 0)
    {
        return -1;
    }
   return 0;
}

int main()
{
    write(1, "This is the client program!\n", strlen("This is the client program!\n"));
    int sd = socket(AF_INET, SOCK_STREAM, 0); // RAW Socket Descriptor
    
    if(sd == -1)
    {
        write(2, "Error: Failed To Initialize a Socket!\n", strlen("Error: Failed To Initialize a Socket!\n"));
        exit(1);
    }

    struct in_addr inadr;
    struct sockaddr_in serv, cli;
    serv.sin_family = AF_INET;
    inadr.s_addr = INADDR_ANY;
    serv.sin_addr = (inadr);
    serv.sin_port = htons(6000); // setting the port no. as 5000
    printf("The port is: %d\n", serv.sin_port);

    int val = connect(sd, (void*)&serv, sizeof(serv));
    if(val == -1) // connection failed
    {
        write(2, "Erro: Failed To Connect!\n", strlen("Erro: Failed To Connect!\n"));
        exit(1);
    }

    write(1, "Client is connected to the server...\n", strlen("Client is connected to the server...\n"));
    int i   = 0;
    int id  = 0;
    while(1)
    {
        char write_buffer[1024] = {0};
        char read_buffer[1024]  = {0};
        if(i != 3) recv(sd, read_buffer, sizeof(read_buffer), 0); // we are still in the phase of logging in
        if(i == 0) // iteration 1 - login
        {
            write(1, read_buffer, sizeof(read_buffer));
            read(0, write_buffer, sizeof(write_buffer));
            send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            id = atoi(write_buffer);
            i = i + 1;
        }
        else if(i == 1) // output having sent the login id
        {
            if(strcmp(read_buffer, "1") == 0)
            {
                write(1, "Incorrect Id!\n", strlen("Incorrect Id!\n"));
                write(1, "Closing the connection...\n", strlen("Closing the connection...\n"));
                exit(1);
            }
            else if(strcmp(read_buffer, "2") == 0)
            {
                write(1, "Account Already Open!\n", strlen("Account Already Open!\n"));
                write(1, "Closing the connection...\n", strlen("Closing the connection...\n"));
                exit(1);
            }
            else // ID has been accepted
            {
                write(1, read_buffer, sizeof(read_buffer));
                read(0, write_buffer, sizeof(write_buffer));
                send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                i = i + 1;
            }
        }
        else if(i == 2) // output having sent the password
        {
            if(strcmp(read_buffer, "1") == 0)
            {
                write(1, "Incorrect Password!\n", strlen("Incorrect Password!\n"));
                write(1, "Closing the connection...\n", strlen("Closing the connection...\n"));
                exit(1);
            }
            else if(strcmp(read_buffer, "2") == 0)
            {
                write(1, "Account Already Open!\n", strlen("Account Already Open!\n"));
                write(1, "Closing the connection...\n", strlen("Closing the connection...\n"));
                exit(1);
            }
            else // password has been accepted
            {
                write(1, read_buffer, sizeof(read_buffer));
                i = i + 1;
            }
        }
        else // main loop (login process was successful)
        {
            if(id != 1) // this is the user account
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
                    if((int)write_buffer[0] == 54 && (int)write_buffer[1] == 10) // the user wants to logout
                    {
                        printf("Closing the connection...\n");
                        exit(1);
                    }
                    else if((int)write_buffer[0] == 51 && (int)write_buffer[1] == 10) // the user wants to know their balance
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
                    else if((int)write_buffer[0] == 53 && (int)write_buffer[1] == 10) // the user wants to know their account details
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
                    else if((int)write_buffer[0] == 49 && (int)write_buffer[1] == 10) // the user wants to deposit into their account
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
                    else if((int)write_buffer[0] == 50 && (int)write_buffer[1] == 10) // the user wants to withdraw from their account
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
                    else if((int)write_buffer[0] == 52 && (int)write_buffer[1] == 10) // the user wants to change their password
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
                        if(check(read_buffer) == -1) write(1, "Failed To Change Password\n", strlen("Failed To Change Password\n"));
                    }
                }
            }
            else // this is the admin's account
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
                    if(((int)write_buffer[0] == 53) && (int)write_buffer[1] == 10) // the admin wants to logout
                    {
                        printf("Closing the connection...\n");
                        exit(1);
                    }
                    else if((int)write_buffer[0] == 54 && (int)write_buffer[1] == 10) // the admin wants to close the server
                    {
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        printf("Closing the connection...\n");
                        exit(1);
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
                        int aux_id = atoi(write_buffer);
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
                        write(1, "Account Id is: ", strlen("Account Id is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);  
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        write(1, "Account Password is: ", strlen("Account Password is: "));
                        write(1, read_buffer, sizeof(read_buffer));
                        write(1, "\n", strlen("\n"));

                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(aux_id != 1)
                        {
                            write(1, "Account Balance is: ", strlen("Account Balance is: "));
                        }
                        else
                        {
                            write(1, "Number of account data-structures in the database are: ", strlen("Number of account data-structures in the database are: "));
                        }
                        write(1, read_buffer, sizeof(read_buffer));
                            write(1, "\n", strlen("\n"));
                        write(1, "++++++++++++++\n", strlen("++++++++++++++\n"));
                    }
                    else if((int)write_buffer[0] == 50 && (int)write_buffer[1] == 10) // admin wants to delete an account
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
                    else if((int)write_buffer[0] == 52 && (int)write_buffer[1] == 10) // admin wants to modify the account password
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
                        write(1, read_buffer, sizeof(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));
                        read(0, write_buffer, sizeof(write_buffer));
                        send(sd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(sd, read_buffer, sizeof(read_buffer), 0);
                        if(check(read_buffer) == -1)
                        {
                            write(1, "Failed to change password!\n", strlen("Failed to change password!\n"));
                            continue;
                        }
                    }
                    else if((int)write_buffer[0] == 49 && (int)write_buffer[1] == 10) // admin wants to add an account
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
                        if(atoi(read_buffer) == 1)
                        {
                            write(1, "Invalid Account Id\n", strlen("Invalid Account Id\n"));
                            continue;
                        }
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