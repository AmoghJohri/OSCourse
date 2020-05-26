/* Importing relevant libraries */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <sys/select.h>

#include "setup.h"

// Defining locks on the open_connections variable. This is to be shared between different threads
pthread_mutex_t mutex_lock1; 

// no lock has been defined on server_on global variable as only the admin has access to it
int server_on               = 1; // this indicates whether the server is switched on of off; if server_on == 0, the server does not accept any further connections
int number_of_accounts      = 0; // this indicates the number of accounts present in the database

int open_connections[1024]  = {0}; // this indicates the account id's which are logged in, a total of 1024 connections are permitted here

// auxillary functions - do not provide much direct functionality to the project
void strreverse(char* begin, char* end) { // reverses a string
	char aux; 
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
	
void itoa(int value, char* str, int base) { // converts an integer into a string
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* wstr=str;
	int sign;
	// Validate base
	if (base<2 || base>35){ *wstr='\0'; return; }
	// Take care of sign
	if ((sign=value) < 0) value = -value;
	// Conversion. Number is reversed.
	do *wstr++ = num[value%base]; while(value/=base);
	if(sign<0) *wstr++='-';
	*wstr='\0';
	// Reverse string
	strreverse(str,wstr-1);	
}

// functions to access the open_connections global variable, this has to be shared between multiple threads and hence, appropriate locking has been defined for reading
int open_con(int id) // this takes in an account id and enters its value in the open_connections array indicating that the account id is active
{
    for(int i = 0; i < 1024; i++)
    {
        if(open_connections[i] == 0)
        {
            open_connections[i] = id;
            
            return 0;
        }
    }
    
    return 1;
}

int close_con(int id) // this takes in an accunt_id and removes its value from the open_connections indicating that the account is now logging out
{
    
    for(int i = 0; i < 1024; i++)
    {
        if(open_connections[i] == id)
        {      
            open_connections[i] = 0;
            
            return 0;
        }
    }
    
    return 1;
}

int is_open(int id) // this takes in an account_id and checks whether it is already logged in or not. Same account ids cannot log in from multiple clients at the same time
{
    for(int i = 0; i < 1024; i++)
    {
        if(open_connections[i] == id)
        {
            
            return 0;
        }
    }
    
    return 1;
}

// routing all the functions to access open_connections from here so that all of them can be simultaneously mutex-locked
int access_connection(int id, int what)
{
    int out = -1;
    pthread_mutex_lock(&mutex_lock1);
    if(what == 1)
        out = open_con(id);
    else if(what == -1)
        out = close_con(id);
    else 
        out = is_open(id);
    pthread_mutex_unlock(&mutex_lock1);
    return out;
}

void pds_init(void) // this initializes our personal data-store. It creates an admin account and 20 other accounts in which 10 are single-user accounts and 20 are joint-user accounts
{
    int fd = open("pds.bin", O_RDONLY); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
    write(2, "Error: Database Failed To Open!\n", strlen("Error: Database Failed To Open!\n"));
    exit(1);
    }
    account_t* pointer = (account_t*)calloc(1, SIZE);
    if(read(fd, pointer, SIZE) != SIZE)
    {
      write(2, "Error: Database Failed To Read!\n", strlen("Error: Database Failed To Read!\n"));
      exit(1);
    }
    number_of_accounts = pointer->balance - 1;
    
    if(lseek(fd, 0, SEEK_SET) != 0)
    {
        write(2, "Error: Lseek Failed!\n", strlen("Error: Lseek Failed!\n"));
        exit(1);
    }
    close(fd);
    return ;
}

int match_password(account_t* pointer, char* pwd) // password verification for an account instance
{
    int i = 0;
    int a;
    int b;
    while(i < 1024)
    {
        a = (int)((pointer->password)[i]);
        b = (int)(pwd[i]);
        if(a != b)
        {
            if(a == 0)
            {
                if(b != 10)
                    return -1;
            }
            else if(a == 10)
            {
                if(b != 0)
                    return -1;
            }
            if(b == 0)
            {
                if(a != 10)
                    return -1;
            }
            else if(b == 10)
            {
                if(a != 0)
                    return -1;
            }
            else
                return -1;
        }
        i = i + 1;
    }
    return 0;
}

int get_len(int* arr) // gets the length of an array (this to know the number of account ids that are associated to an account)
{
    int counter = 0;
    for(int i = 0; i < ACC; i++)
    {
        if(arr[i] > 0)
            counter = counter + 1;
    }
    return counter;
}

void get_info(unsigned int id, account_t** pointer) // takes in the account id and returns the info about the account in an account_t struct; if the account is not present, the value of attribute "this_id" is set to 0
{
    struct flock lock; // putting a read lock on the file

    int fd = open("pds.bin", O_RDONLY); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      write(2, "Error: Database Failed To Open!\n", strlen("Error: Database Failed To Open!\n"));
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;
    int iter_total = number_of_accounts + 1;
    for(int i = 0; i < iter_total; i++)
    {
        if(tag != 0) // already found the account so we do not want to be reading any further
            break;

        int val = read(fd, *pointer, SIZE);
        if(val != SIZE)
        {
            if(val == 0)
                continue;
            write(2, "Error: Read Failed\n", strlen("Error: Read Failed\n"));
            exit(1);
        }
        unsigned int* arr = (*pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] == id)
            {
                (*pointer)->this_id = arr[counter];
                tag = 1;
                counter ++;
                break;
            }
            counter ++;
        }
        if(counter == 0)    iter_total ++;

    }
    if(tag == 0)
    {
        memset(*pointer, 0, sizeof(*pointer));
    }

    // unlocking the file
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);

    // close(fd); we will not be doing this as this is a multithreader program
    return ;
}

int augment_balance(float new, int id) // takes in the amount which is to be added to the balance and the corresponding id of the acccount, and does the needfull
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, SIZE);

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      write(2, "Error: Database Failed To Open!\n", strlen("Error: Database Failed To Open!\n"));
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;
    int iter_total = number_of_accounts + 1;
    for(int i = 0; i < iter_total; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        int val = read(fd, pointer, SIZE);
        if(val != SIZE)
        {
            if(val == 0)
                continue;   
            write(2, "Error: Read Failed\n", strlen("Error: Read Failed\n"));
            exit(1);
        }
        unsigned int* arr = (pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] == id)
            {
                pointer->this_id = id;
                if(pointer->balance + new < 0)
                    pointer->this_id = 0;
                else
                    pointer->balance = pointer->balance + new;
                tag = 1;
                counter ++;
                break;
            }
            counter ++;
        }
        if(counter == 0) iter_total ++;

    }
    if(tag == 0) // account was not found
    {  
        return -1;
    }
    else if(pointer->this_id == 0) // insufficient balance
    {   
        return -2;
    }
    else   
    {
        int pos = lseek(fd, -1*SIZE, SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a write lock on the record
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the record */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is position of the record
            lock2.l_len = SIZE; 
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, pointer, SIZE);
            // unlocking the record
            lock2.l_type = F_UNLCK;
            fcntl (fd, F_SETLKW, &lock2);
        }
    }
    // unlocking the file
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
    
    // close(fd); we will not be doing this as this is a multithreader program
    return 0;
}

int change_password(char* new, int id) // takes the new password and the associated account id and changes the password
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, SIZE);

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      
      write(2, "Error: Database Failed To Open!\n", strlen("Error: Database Failed To Open!\n"));
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;
    int iter_total = number_of_accounts + 1;
    for(int i = 0; i < iter_total; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        int val = read(fd, pointer, SIZE);
        if(val != SIZE)
        {
            if(val == 0)
                continue; 
            write(2, "Error: Read Failed\n", strlen("Error: Read Failed\n"));
            exit(1);
        }
        unsigned int* arr = (pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] == id)
            {
                pointer->this_id = id;
                strcpy(pointer->password, new);
                tag = 1;
                counter ++;
                break;
            }
            counter ++;
        }
        if(counter == 0) iter_total ++;

    }
    if(tag == 0) // the account was not found
    {   
        return -1;
    }
    else
    {
        int pos = lseek(fd, -1*SIZE, SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a write lock on the record
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the record. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is for the current record
            lock2.l_len = SIZE; 
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, pointer, SIZE);
            // unlocking the file
            lock2.l_type = F_UNLCK;
            fcntl (fd, F_SETLKW, &lock2);
        }
    }
    // unlocking the file
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
    
    // close(fd); we will not be doing this as this is a multithreader program
    return 0;
}

int delete_account(int id) // takes an id and deletes the account corresponding to it
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, SIZE);

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      
      write(2, "Error: Database Failed To Open!\n", strlen("Error: Database Failed To Open!\n"));
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;
    int iter_total = number_of_accounts + 1;
    for(int i = 0; i < iter_total; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        int val = read(fd, pointer, SIZE);
        if(val != SIZE)
        {
            if(val == 0)
                continue; 
            write(2, "Error: Read Failed\n", strlen("Error: Read Failed\n"));
            exit(1);
        }
        unsigned int* arr = (pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] == id)
            {
                pointer->this_id = id;
                tag = 1;
                counter ++;
                break;
            }
            counter ++;
        }
        if(counter == 0) iter_total ++;

    }
    if(tag == 0) // account was not found 
    {
        return 1;
    }
    else
    {
        int pos = lseek(fd, -1*SIZE, SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a write lock on the record
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is position of the record
            lock2.l_len = SIZE; 
            fcntl (fd, F_SETLKW, &lock2);
            memset(pointer, 0, sizeof(pointer));
            write(fd, pointer, SIZE);
            // unlocking the file
            lock2.l_type = F_UNLCK;
            fcntl (fd, F_SETLKW, &lock2);
            number_of_accounts --;

            lseek(fd, 0, SEEK_SET);
            read(fd, pointer, SIZE);
            pointer->balance = number_of_accounts + 1;
            lseek(fd, 0, SEEK_SET);

            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = 0;         // starting offset is zero
            lock2.l_len = SIZE;
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, pointer, SIZE);
            // unlocking the file
            lock2.l_type = F_UNLCK;
            fcntl (fd, F_SETLKW, &lock2);
            

        }
    }
    // unlocking the file
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
    
    // close(fd); we will not be doing this as this is a multithreader program
    return 0;
}

int add_account(account_t* new) // takes the account information and adds it into the database
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, SIZE);

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      write(2, "Error: Database Failed To Open!\n", strlen("Error: Database Failed To Open!\n"));
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;

    int iter_total = number_of_accounts + 1;
    for(int i = 0; i < iter_total; i++)
    {
        int val = read(fd, pointer, SIZE);
        if(val != SIZE)
        {
            if(val == 0)
                continue;
            write(2, "Error: Read Failed\n", strlen("Error: Read Failed\n"));
            exit(1);
        }
        if(i == 0)
            continue;
        unsigned int* arr = (pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] != 0)
            {
                tag = 1;
                counter ++;
                break;
            }
            counter ++;
        }
        if(counter == 0) // found a hole
        {
            tag = 1;
            break;
        }
    }
    struct flock lock2;
    if(tag == 0) // there were no empty accounts found, hence, the account is being added to the end; 
    {
         /* Initialize the flock structure. */
        memset (&lock2, 0, sizeof(lock));
        lock2.l_type = F_WRLCK;
        /* Place a write lock on the file. */
        lock2.l_whence = SEEK_SET; // offset base is start of the file
        lock2.l_start = 0;         // starting offset is zero
        lock2.l_len = SIZE; 
        fcntl (fd, F_SETLKW, &lock2);
        write(fd, new, SIZE);
        lock2.l_type = F_UNLCK;
        fcntl (fd, F_SETLKW, &lock2);
    }
    else // an empty account (hole) was found in the database, so the new account's information in being put in there
    {
        int pos = lseek(fd, -1*SIZE, SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a write lock on the record
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is position of the record
            lock2.l_len = SIZE; 
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, new, SIZE);
            // unlocking the file
            lock2.l_type = F_UNLCK;
            fcntl (fd, F_SETLKW, &lock2);
        }
    }
    int pos = lseek(fd, 0, SEEK_SET);
     /* Initialize the flock structure. */
    memset (&lock2, 0, sizeof(lock));
    lock2.l_type = F_WRLCK;
    /* Place a write lock on the file. */
    lock2.l_whence = SEEK_SET; // offset base is start of the file
    lock2.l_start = 0;       
    read(fd, pointer, SIZE);
    pointer->balance = pointer->balance + 1.;
    pos = lseek(fd, 0, SEEK_SET);
    fcntl (fd, F_SETLKW, &lock2);
    write(fd, pointer, SIZE);
    lock2.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock2);
    number_of_accounts ++;
    // unlocking the file
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);
    // close(fd); we will not be doing this as this is a multithreader program
    return 0;
}

void* login(void* nsd_) // this is the main function, everything that the server does with respect to a particular client goes on in here
{
    char id[2]           = {0};
    char password[LEN]   = {0};         // LEN = 1024
    int nsd              = *(int*)nsd_;
    int i                = 0;
    int tag              = 0;
    unsigned int open_id = 0;

    account_t* pointer   = (account_t*)calloc(1, SIZE); // this will contain all the information of the account being dealt with

    while(1)
    {
        char write_buffer[1024] = {0}; // this will be used to send messages to the client
        char read_buffer[1024]  = {0}; // this will be used to receive messages from the client

        if(i == 0) // iteration 1, here the login id is verified
        {
            strcpy(write_buffer, "Enter the login id: ");
            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            recv(nsd, read_buffer, sizeof(read_buffer), 0);
            get_info(atoi(read_buffer), &pointer);
            if(pointer->this_id == 0)            // login id was not found
                tag = -1;
            if(access_connection(pointer->this_id, 0) == 0)   // login id added to the array of open connections
            {
                if(tag == -1)
                    access_connection(pointer->this_id, -1); // if the login id was not found, it is removed from the set of open connections
                else                             // the login is already open
                    tag = -2;
            }
            else                                 // the login id was found and it was not already open and hence, we can procede further
                open_id = pointer->this_id;
            i = i + 1;
        }
        else if(i == 1)                    // second iteration, the password is verified here
        {
            if(tag == -1)                  // incorrect id
            {
                strcpy(write_buffer, "1"); // whenever there is an error, a value greater than 0 is sent to the client, this will indicate to the client that something went wrong
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);                // as something went wrong, we close the server
                
                return NULL;

            }
            else if(tag == -2)             // id already open
            {
                strcpy(write_buffer, "2");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);
                
                return NULL;
            }
            else
            {
                strcpy(write_buffer, "Enter the password: ");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                recv(nsd, read_buffer, sizeof(read_buffer), 0); // the password is received by the server
                get_info(open_id, &pointer);                    // this is performed in case the password has been changed by other users in a joint account scenario (or changed by the admin)
                tag = match_password(pointer, read_buffer);     // matching the password
                if(access_connection(open_id, 0) == 0)
                    tag = -2;                                   // if during this time, the same id got opened from another client process, we raise an error
                i = i + 1;
            }
        }
        else if(i == 2)         // completing the login
        {
            if(tag == -1)       // if the password did not match
            {
                strcpy(write_buffer, "1");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);
                
                return NULL;
            }
            else if(tag == -2)  // if the id is already open
            {
                strcpy(write_buffer, "2");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);
                
                return NULL;
            }
            else               // we reach here after a successful login
            {
                access_connection(open_id, 1);
                strcpy(write_buffer, "Login Successful!\n");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                i = i + 1;
            }
        }
        else // main loop (can only be reached after a successful login)
        {
            while(1)
            {
                if(open_id != 1) // this is a user account (id == 1 has been reserved for the admin)
                {
                    memset(read_buffer, 0, sizeof(read_buffer)); 
                    memset(write_buffer, 0, sizeof(write_buffer)); 

                    // providing the user with various options
                    strcpy(write_buffer, "Press 1 to Deposit\nPress 2 to Withdraw\nPress 3 for Balance Enquiry\nPress 4 for Password Change\nPress 5 to view details\nPress 6 to exit\nEnter your option: ");
                    send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

                    recv(nsd, read_buffer, sizeof(read_buffer), 0); // receiving the user input

                    if((int)read_buffer[0] == 54 && (int)read_buffer[1] == 10) // this input indicates that the user wants to logout
                    {
                        access_connection(open_id, -1);
                        close(nsd);
                        
                        return NULL;
                    }
                    // options which only require reading
                    else if((int)read_buffer[0] == 51 && (int)read_buffer[1] == 10) // this input indicates that the user wants to know the account balance
                    {
                        get_info(open_id, &pointer);

                        memset(write_buffer, 0, sizeof(write_buffer));
                        gcvt(pointer->balance, 10, write_buffer); 
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                    else if((int)read_buffer[0] == 53 && (int)read_buffer[1] == 10) // this input indicates that the user wants to know the account details
                    {
                        get_info(open_id, &pointer);

                        memset(write_buffer, 0, sizeof(write_buffer));
                        itoa(pointer->this_id, write_buffer, 10);
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        
                        memset(write_buffer, 0, sizeof(write_buffer));
                        if(get_len(pointer->id) == 1)
                                strcpy(write_buffer, "This is a single account");
                            else
                                strcpy(write_buffer, "This is a joint account");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, pointer->password);
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        
                        memset(write_buffer, 0, sizeof(write_buffer));
                        gcvt(pointer->balance, 10, write_buffer); 
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                    // options which require writing
                    else if((int)read_buffer[0] == 49 && (int)read_buffer[1] == 10) // this input indicates that the user wants to deposit 
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the amount: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);

                        augment_balance(strtof(read_buffer, NULL), open_id);
                    }
                    else if((int)read_buffer[0] == 50 && (int)read_buffer[1] == 10) // this input indicates that the user wants to withdraw
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the amount: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);

                        memset(write_buffer, 0, sizeof(write_buffer));
                        if(augment_balance(-1*(strtof(read_buffer, NULL)), open_id) == -2)
                            strcpy(write_buffer, "1");
                        else
                            strcpy(write_buffer, "0");
                        send(nsd, write_buffer, sizeof(read_buffer), MSG_CONFIRM);
                    }
                    else if((int)read_buffer[0] == 52 && (int)read_buffer[1] == 10) // this input indicates that the user wants to change the account password
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the new-password: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        for(int i = 0; i < 1024; i++)
                        {
                            if((int)read_buffer[i] == 10)
                                read_buffer[i] = (char)0;
                        }
                        if(change_password(read_buffer, open_id) == 0)
                            strcpy(write_buffer, "1");
                        else
                            strcpy(write_buffer, "0");
                        memset(write_buffer, 0, sizeof(write_buffer));
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                    else // this indicates an invalid input and in this case the user is prompted to provide an input again
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "1");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                }
                else if(open_id == 1) // admin
                {
                    account_t* aux = (account_t*)calloc(1, SIZE); // this will contain all the information about the accounts admin wishes to access
                    memset(read_buffer, 0, sizeof(read_buffer));
                    memset(write_buffer, 0, sizeof(write_buffer));

                    // these are the various options provided to the admin
                    strcpy(write_buffer, "Press 1 To Add an Account\nPress 2 To Delete an Account\nPress 3 To Get Account Details\nPress 4 To Modify Account Password\nPress 5 To Exit\nPress 6 To Shut the Server Down\nEnter your option: ");
                    send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    
                    // the input is received from the admin
                    recv(nsd, read_buffer, sizeof(read_buffer), 0);
                    if((int)read_buffer[0] == 53 && (int)read_buffer[1] == 10) // this input indicates that the admin wishes to logout
                    {
                        access_connection(open_id, -1);
                        close(nsd);
                        
                        return NULL;
                    }
                    else if((int)read_buffer[0] == 51 && (int)read_buffer[1] == 10) // this input indicates that the admin wishes to get information about a particular account
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the account id: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);

                        get_info(atoi(read_buffer), &aux);
                        memset(write_buffer, 0, sizeof(write_buffer));

                        if(aux->this_id == 0.0)
                        {
                            strcpy(write_buffer, "1");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        }
                        else
                        {
                            strcpy(write_buffer, "0");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

                            memset(write_buffer, 0, sizeof(write_buffer));
                            itoa(aux->this_id, write_buffer, 10);
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

                            memset(write_buffer, 0, sizeof(write_buffer));
                            if(get_len(aux->id) == 1)
                                strcpy(write_buffer, "This is a single account");
                            else
                                strcpy(write_buffer, "This is a joint account");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

                            memset(write_buffer, 0, sizeof(write_buffer));
                            strcpy(write_buffer, aux->password);
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);

                            memset(write_buffer, 0, sizeof(write_buffer));
                            gcvt(aux->balance, 10, write_buffer); 
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        }
                    }
                    else if((int)read_buffer[0] == 50 && (int)read_buffer[1] == 10) // this input indicates that the admin wants to delete an account
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the account id: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        get_info(atoi(read_buffer), &aux);
                        while(1)
                        {
                            unsigned int* array = aux->id;
                            int counter = 0;
                            int tag = 0;
                            while(array[counter] != 0)
                            {
                                while(access_connection(array[counter], 0) == 0)
                                    tag = 1;
                                counter = counter + 1;
                            }
                            if(tag == 0)
                                break;
                        }
                        int val = delete_account(atoi(read_buffer));
                        memset(write_buffer, 0, sizeof(write_buffer));

                        if(val == 1)
                        {
                            strcpy(write_buffer, "1");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        }
                        else
                        {
                            strcpy(write_buffer, "0");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM); 
                            unsigned int* array = aux->id;
                            int counter = 0;
                            while(array[counter] != 0)
                            {
                                access_connection(array[counter], -1);
                                counter ++;
                            }

                        }
                    }
                    else if((int)read_buffer[0] == 49 && (int)read_buffer[1] == 10) // this input indicates that the admin wants to create an account
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the number of people sharing (1 to 5): ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        int num_of_accounts = atoi(read_buffer);
                        memset(write_buffer, 0, sizeof(write_buffer));
                        if(num_of_accounts <= 5 && num_of_accounts >= 1)
                            strcpy(write_buffer, read_buffer);
                        else // invalid number of joint accounts
                        {
                            strcpy(write_buffer, "-1");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                            continue;
                        }
                        int tag = 0;
                        account_t* new = (account_t*)calloc(1, SIZE);
                        account_t* aux = (account_t*)calloc(1, SIZE);
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM); // sending the number of times looping is required;
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the account ids (one after the other): ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        for(int i = 0; i < num_of_accounts; i++)
                        {
                            memset(read_buffer, 0, sizeof(read_buffer));
                            recv(nsd, read_buffer, sizeof(read_buffer), 0);
                            memset(write_buffer, 0, sizeof(write_buffer));
                            get_info(atoi(read_buffer), &aux);
                            if(atoi(read_buffer) < 2 || (aux->this_id) != 0)
                                strcpy(write_buffer, "1");
                            else
                                strcpy(write_buffer, "0");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                            if(atoi(read_buffer) < 2 || (aux->this_id) != 0)
                            {
                                tag = 1;
                                free(new);
                                break;
                            }
                            (new->id)[i] = atoi(read_buffer);
                        }
                        int counter = 0;
                        while((new->id)[counter] != 0)
                        {
                            int counter2 = counter + 1;
                            while((new->id)[counter2] != 0)
                            {
                                if((new->id)[counter2] == (new->id)[counter])
                                    tag = 1;
                                counter2 ++;
                            }
                            counter ++;
                        }
                        if(tag == 1)
                        {
                            memset(write_buffer, 0, sizeof(write_buffer));
                            strcpy(write_buffer, "1");
                            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                            continue;
                        }
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the password: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        strcpy(new->password,read_buffer);
                        counter = 0;
                        while((new->password)[counter] != 0)
                        {
                            if((new->password)[counter] == 10)
                                (new->password)[counter] = 0;
                            counter ++;
                        }
                        new->balance = 0.0;
                        add_account(new);
                    }
                    else if((int)read_buffer[0] == 52 && (int)read_buffer[1] == 10) // this input indicates that the admin wishes to modify the account password
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the account id: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        memset(write_buffer, 0, sizeof(write_buffer));
                        get_info(atoi(read_buffer), &aux);
                        if(aux->this_id == 0)
                            strcpy(write_buffer, "1");
                        else
                            strcpy(write_buffer, "Enter the new password: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        if(aux->this_id == 0)
                            continue;
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        for(int i = 0; i < 1024; i++)
                        {
                            if((int)read_buffer[i] == 10)
                                read_buffer[i] = (char)0;
                        }
                        memset(write_buffer, 0, sizeof(write_buffer));
                        if(change_password(read_buffer, aux->this_id) == 0)
                            strcpy(write_buffer, "0");
                        else
                            strcpy(write_buffer, "1");
                        
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);       
                    }
                    else if((int)read_buffer[0] == 54 && (int)read_buffer[1] == 10) // this input indicates that the admin wishes to close the server
                    {
                        int sd = socket(AF_INET, SOCK_STREAM, 0);
                        struct in_addr inadr;
                        struct sockaddr_in serv, cli;
                        serv.sin_family = AF_INET;
                        inadr.s_addr = INADDR_ANY;
                        serv.sin_addr = (inadr);
                        serv.sin_port = htons(6000);
                        server_on = 0;
                        int val = connect(sd, (void*)&serv, sizeof(cli));
                        if(val == -1)
                            server_on = 1;
                        close(nsd);
                        access_connection(1, -1);
                        return NULL;
                    }
                    else // this indicates an invalid input and in this case the admin is prompted to provide an input again
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "1");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                }
            }
        }
    }
}

int main()
{   
    // printf("This is the server program!\n");
    int sd = socket(AF_INET, SOCK_STREAM, 0); // RAW Socket Descriptor
    if(sd == -1)
    {
        write(2, "Error: Failed To Initialize the Socket!\n", strlen("Error: Failed To Initialize the Socket!\n"));
        exit(1);
    }
    struct in_addr inadr;
    struct sockaddr_in serv, cli;
    int addrlen = sizeof(cli);
    serv.sin_family = AF_INET;
    inadr.s_addr = INADDR_ANY; // since the communication is taking place over the same device
    serv.sin_addr = (inadr);
    serv.sin_port = htons(6000); // setting the port no. as 5000
    // printf("The port is: %d\n", serv.sin_port); 

    int enable = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        write(2, "Error: setsockopt(SO_REUSEADDR) failed", strlen("Error: setsockopt(SO_REUSEADDR) failed"));

    int val = bind(sd, (void*)&serv, sizeof(serv));
    if(val == -1)
    {
        if(errno == EADDRINUSE)
        {
            write(2, "Error: Given Address Is Already In Use!\n", strlen("Error: Given Address Is Already In Use!\n"));
            exit(1);
        }
        else
        {
            write(2, "Error: Bind Failed!\n", strlen("Error: Bind Failed!\n"));
            exit(1);
        }
    }

    val = listen(sd, 0);
    if(val  == -1)
    {
        write(2, "Error: Listen Failed\n", strlen("Error: Listen Failed\n"));
        exit(1);
    }

    if (pthread_mutex_init(&mutex_lock1, NULL) != 0) { 
        write(2, "\n mutex init has failed\n", strlen("\n mutex init has failed\n")); 
        exit(1); 
    }  
    pds_init();

    pthread_t thread_id[1024]; // maximum number of thread supported
    int counter = 0;
    int nsd = 0;

    while(1)
    {            
        nsd = accept(sd, (void*)&cli, (socklen_t*)&addrlen);
        if(server_on == 0) // this is once the server is signalled switched off; it does not accept any more connections but lets the ongoing connections run to completion
        {
            break;
        }
        if(nsd == -1)
        {
            write(2, "Error: Connection Failed To Establish!\n", strlen("Error: Connection Failed To Establish!\n"));
        }
        else
        {
            pthread_create( &thread_id[counter] , NULL ,  login , (void*) &nsd);
            counter++;
            if(counter == 1024)
            {
                write(2, "Maximum thread count exceded!\n", strlen("Maximum thread count exceded!\n"));
            }
        }
    }   
    for(unsigned long int i = 0; i < counter; i++)
    {
        pthread_join(thread_id[i], NULL); // waiting for all the active threads
    }
    shutdown(nsd, SHUT_RDWR); // shutting the server down
    
    pthread_mutex_destroy(&mutex_lock1); 
    return 0;
}  