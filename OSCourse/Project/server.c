#include <sys/types.h>          
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include "setup.h"

pthread_mutex_t mutex_lock1; 
pthread_mutex_t mutex_lock2; 

int server_on = 1;
int number_of_accounts = -1;
int open_connections[1024] = {0};

void strreverse(char* begin, char* end) {
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
	
void itoa(int value, char* str, int base) {
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

int open_con(int id)
{
    for(int i = 0; i < 1024; i++)
    {
        if(open_connections[i] == 0)
        {
            pthread_mutex_lock(&mutex_lock1); 
            open_connections[i] = id;
            pthread_mutex_unlock(&mutex_lock1); 
            return 0;
        }
    }
    return 1;
}

int close_con(int id)
{
    for(int i = 0; i < 1024; i++)
    {
        if(open_connections[i] == id)
        {
            pthread_mutex_lock(&mutex_lock1); 
            open_connections[i] = 0;
            pthread_mutex_unlock(&mutex_lock1); 
            return 0;
        }
    }
    return 1;
}

int is_open(int id)
{
    for(int i = 0; i < 1024; i++)
    {
        if(open_connections[i] == id)
            return 0;
    }
    return 1;
}

void pds_init(void)
{
    int fd = open("pds.bin", O_RDONLY); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
    printf("Error: Database Failed To Open!\n");
    exit(1);
    }
    account_t* pointer = (account_t*)calloc(1, sizeof(SIZE));
    if(read(fd, pointer, SIZE) != SIZE)
    {
      printf("Error: Database Failed To Read!\n");
      exit(1);
    }
    pthread_mutex_lock(&mutex_lock2); 
    number_of_accounts = pointer->balance - 1;
    pthread_mutex_unlock(&mutex_lock2); 
    free(pointer);
    if(lseek(fd, 0, SEEK_SET) != 0)
    {
        printf("Error: Lseek Failed!\n");
        exit(-1);
    }
    close(fd);
    return ;
}

int match_password(account_t* pointer, char* pwd)
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

int get_len(int* arr)
{
    int counter = 0;
    for(int i = 0; i < 5; i++)
    {
        if(arr[i] > 0)
            counter = counter + 1;
    }
    return counter;
}

void get_info(unsigned int id, account_t** pointer)
{
    struct flock lock; // putting a read lock on the file

    int fd = open("pds.bin", O_RDONLY); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      printf("Error: Database Failed To Open!\n");
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;

    for(int i = 0; i < number_of_accounts; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        if(read(fd, *pointer, sizeof(account_t)) != sizeof(account_t))
        {
            printf("Error: Read Failed\n");
            exit(-1);
        }
        unsigned int* arr = (*pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] == id)
            {
                (*pointer)->this_id = arr[counter];
                tag = 1;
                break;
            }
            counter ++;
        }

    }
    if(tag == 0)
        (*pointer)->this_id = 0;

    // unlocking the file
    lock.l_type = F_UNLCK;
    fcntl (fd, F_SETLKW, &lock);

    // close(fd); we will not be doing this as this is a multithreader program
    return ;
}

int augment_balance(float new, int id)
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, sizeof(account_t));

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      printf("Error: Database Failed To Open!\n");
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;

    for(int i = 0; i < number_of_accounts; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        if(read(fd, pointer, sizeof(account_t)) != sizeof(account_t))
        {
            printf("Error: Read Failed\n");
            exit(-1);
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
                break;
            }
            counter ++;
        }

    }
    if(tag == 0)
        return -1;
    else if(pointer->this_id == 0) // insufficient balance
        return -2;
    else// otherwise we have balance lower than 0
    {
        int pos = lseek(fd, -1*sizeof(account_t), SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a read lock on the file
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is zero
            lock2.l_len = sizeof(account_t); 
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, pointer, sizeof(account_t));
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

int change_password(char* new, int id)
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, sizeof(account_t));

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      printf("Error: Database Failed To Open!\n");
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;

    for(int i = 0; i < number_of_accounts; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        if(read(fd, pointer, sizeof(account_t)) != sizeof(account_t))
        {
            printf("Error: Read Failed\n");
            exit(-1);
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
                break;
            }
            counter ++;
        }

    }
    if(tag == 0)
        return -1;
    else
    {
        int pos = lseek(fd, -1*sizeof(account_t), SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a read lock on the file
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is zero
            lock2.l_len = sizeof(account_t); 
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, pointer, sizeof(account_t));
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

int delete_account(int id)
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, sizeof(account_t));

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      printf("Error: Database Failed To Open!\n");
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;

    for(int i = 0; i < number_of_accounts; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        if(read(fd, pointer, sizeof(account_t)) != sizeof(account_t))
        {
            printf("Error: Read Failed\n");
            exit(-1);
        }
        unsigned int* arr = (pointer)->id;
        int counter = 0;
        while(arr[counter] != 0)
        {
            if(arr[counter] == id)
            {
                pointer->this_id = id;
                tag = 1;
                break;
            }
            counter ++;
        }

    }
    if(tag == 0)
        return -1;
    else
    {
        int pos = lseek(fd, -1*sizeof(account_t), SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a read lock on the file
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is zero
            lock2.l_len = sizeof(account_t); 
            fcntl (fd, F_SETLKW, &lock2);
            memset(pointer->id, 0, sizeof(pointer->id));
            write(fd, pointer, sizeof(account_t));
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

int add_account(account_t* new)
{
    struct flock lock; // putting a read lock on the file

    account_t* pointer = (account_t*)calloc(1, sizeof(account_t));

    int fd = open("pds.bin", O_RDWR); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      printf("Error: Database Failed To Open!\n");
      exit(1);
    }

    /* Initialize the flock structure. */
    memset (&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    /* Place a write lock on the file. */
    fcntl (fd, F_SETLKW, &lock);

    int tag = 0;

    for(int i = 0; i < number_of_accounts; i++)
    {
        if(tag != 0) // already found the accont so we do not want to be reading any further
            break;

        if(read(fd, pointer, sizeof(account_t)) != sizeof(account_t))
        {
            printf("Error: Read Failed\n");
            exit(-1);
        }
        if((pointer->id)[0] == 0)
        {
            tag = 1;
            break;
        }
    }

    if(tag == 0)
    {
        write(fd, new, sizeof(account_t));
        int pos = lseek(fd, 0, SEEK_SET);
        read(fd, pointer, sizeof(account_t));
        pointer->balance = pointer->balance + 1.;
        pos = lseek(fd, 0, SEEK_SET);
        write(fd, pointer, sizeof(account_t));
        pthread_mutex_lock(&mutex_lock2); 
        number_of_accounts ++;
        pthread_mutex_unlock(&mutex_lock2); 
    }
    else
    {
        int pos = lseek(fd, -1*sizeof(account_t), SEEK_CUR); // move one space back
        {
            struct flock lock2; // putting a read lock on the file
             /* Initialize the flock structure. */
            memset (&lock2, 0, sizeof(lock));
            lock2.l_type = F_WRLCK;
            /* Place a write lock on the file. */
            lock2.l_whence = SEEK_SET; // offset base is start of the file
            lock2.l_start = pos;         // starting offset is zero
            lock2.l_len = sizeof(account_t); 
            fcntl (fd, F_SETLKW, &lock2);
            write(fd, new, sizeof(account_t));
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

void* login(void* nsd_)
{
    char id[2] = {0};
    char password[LEN] = {0};
    int nsd = *(int*)nsd_;
    // printf("NSD: %d\n", nsd);

    int i = 0;
    int tag = 0;

    unsigned int open_id = 0;

    account_t* pointer = (account_t*)calloc(1, sizeof(account_t));
    while(1)
    {
        char write_buffer[1024] = {0};
        char read_buffer[1024] = {0};
        if(i == 0) // iteration 1
        {
            strcpy(write_buffer, "Enter the login id: ");
            send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
            recv(nsd, read_buffer, sizeof(read_buffer), 0);
            get_info(atoi(read_buffer), &pointer);
            if(pointer->this_id == 0)
                tag = -1;
            if(is_open(pointer->this_id) == 0)
            {
                if(tag == -1)
                    close_con(pointer->this_id);
                else 
                    tag = -2;
            }
            else
                open_id = pointer->this_id;
            i = i + 1;
        }
        else if(i == 1) // second iteration
        {
            if(tag == -1) // incorrect id
            {
                strcpy(write_buffer, "1");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);
                return NULL;

            }
            else if(tag == -2) // id already open
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
                recv(nsd, read_buffer, sizeof(read_buffer), 0);
                get_info(open_id, &pointer);
                tag = match_password(pointer, read_buffer);
                if(is_open(open_id) == 0)
                    tag = -2;
                i = i + 1;
            }
        }
        else if(i == 2) // completing the login
        {
            if(tag == -1)
            {
                strcpy(write_buffer, "1");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);
                return NULL;
            }
            else if(tag == -2) // id already open
            {
                strcpy(write_buffer, "2");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                close(nsd);
                return NULL;
            }
            else
            {
                open_con(open_id);
                strcpy(write_buffer, "Login Successful!\n");
                send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                i = i + 1;
            }
        }
        else // main loop
        {
            while(1)
            {
                if(open_id != 1)// this is a user account
                {
                    memset(read_buffer, 0, sizeof(read_buffer));
                    memset(write_buffer, 0, sizeof(write_buffer));
                    strcpy(write_buffer, "Press 1 to Deposit\nPress 2 to Withdraw\nPress 3 for Balance Enquiry\nPress 4 for Password Change\nPress 5 to view details\nPress 6 to exit\nEnter your option: ");
                    send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    recv(nsd, read_buffer, sizeof(read_buffer), 0);
                    if((int)read_buffer[0] == 54 && (int)read_buffer[1] == 10)
                    {
                        close_con(open_id);
                        close(nsd);
                        return NULL;
                    }
                    // options which only require reading
                    else if((int)read_buffer[0] == 51 && (int)read_buffer[1] == 10)
                    {
                        get_info(open_id, &pointer);

                        memset(write_buffer, 0, sizeof(write_buffer));
                        gcvt(pointer->balance, 10, write_buffer); 
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                    else if((int)read_buffer[0] == 53 && (int)read_buffer[1] == 10)
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
                    else if((int)read_buffer[0] == 49 && (int)read_buffer[1] == 10)
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the amount: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);

                        augment_balance(strtof(read_buffer, NULL), open_id);
                    }
                    else if((int)read_buffer[0] == 50 && (int)read_buffer[1] == 10)
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
                    else if((int)read_buffer[0] == 52 && (int)read_buffer[1] == 10)
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
                    else
                    {
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "1");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    }
                }
                else if(open_id == 1) // admin
                {
                    account_t* aux = (account_t*)calloc(1, sizeof(account_t));
                    memset(read_buffer, 0, sizeof(read_buffer));
                    memset(write_buffer, 0, sizeof(write_buffer));
                    strcpy(write_buffer, "Press 1 To Add an Account\nPress 2 To Delete an Account\nPress 3 To Get Account Details\nPress 4 To Modify Account Password\nPress 5 To Exit\nPress 6 To Shut the Server Down\nEnter your option: ");
                    send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                    recv(nsd, read_buffer, sizeof(read_buffer), 0);
                    if((int)read_buffer[0] == 53 && (int)read_buffer[1] == 10)
                    {
                        close_con(open_id);
                        close(nsd);
                        return NULL;
                    }
                    else if((int)read_buffer[0] == 51 && (int)read_buffer[1] == 10)
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
                    else if((int)read_buffer[0] == 50 && (int)read_buffer[1] == 10)
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
                                while(is_open(array[counter]) == 0)
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
                                close_con(array[counter]);
                                counter ++;
                            }

                        }
                    }
                    else if((int)read_buffer[0] == 49 && (int)read_buffer[1] == 10)
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
                        account_t* new = (account_t*)calloc(1, sizeof(account_t));
                        account_t* aux = (account_t*)calloc(1, sizeof(account_t));
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
                        if(tag == 1)
                            continue;
                        memset(write_buffer, 0, sizeof(write_buffer));
                        strcpy(write_buffer, "Enter the password: ");
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        strcpy(new->password,read_buffer);
                        int counter = 0;
                        while((new->password)[counter] != 0)
                        {
                            if((new->password)[counter] == 10)
                                (new->password)[counter] = 0;
                            counter ++;
                        }
                        new->balance = 0.0;
                        add_account(new);
                    }
                    else if((int)read_buffer[0] == 52 && (int)read_buffer[1] == 10)
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
                        memset(read_buffer, 0, sizeof(read_buffer));
                        recv(nsd, read_buffer, sizeof(read_buffer), 0);
                        for(int i = 0; i < 1024; i++)
                        {
                            if((int)read_buffer[i] == 10)
                                read_buffer[i] = (char)0;
                        }
                        if(change_password(read_buffer, aux->this_id) == 0)
                            strcpy(write_buffer, "1");
                        else
                            strcpy(write_buffer, "0");
                        memset(write_buffer, 0, sizeof(write_buffer));
                        send(nsd, write_buffer, sizeof(write_buffer), MSG_CONFIRM);       
                    }
                    else if((int)read_buffer[0] == 54 && (int)read_buffer[1] == 10)
                    {
                        server_on = 0;
                        int sd = socket(AF_INET, SOCK_STREAM, 0);
                        struct in_addr inadr;
                        struct sockaddr_in serv, cli;
                        serv.sin_family = AF_INET;
                        serv.sin_addr = (inadr);
                        serv.sin_port = htons(6000);
                        connect(sd, (void*)&serv, sizeof(cli));
                        return NULL;
                    }
                    else
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
        printf("Error: Failed To Initialize the Socket!\n");
        exit(-1);
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
        printf("Error: setsockopt(SO_REUSEADDR) failed");

    int val = bind(sd, (void*)&serv, sizeof(serv));
    if(val == -1)
    {
        if(errno == EADDRINUSE)
        {
            printf("Error: Given Address Is Already In Use!\n");
            exit(-1);
        }
        else
        {
            printf("Error: Bind Failed!\n");
            exit(-1);
        }
    }

    val = listen(sd, 3);
    if(val  == -1)
    {
        printf("Error: Listen Failed\n");
        exit(-1);
    }

    //  printf("Server is up and running...\n");

    if (pthread_mutex_init(&mutex_lock1, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    }  
    if (pthread_mutex_init(&mutex_lock2, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    pds_init();

    pthread_t thread_id[1024];
    int counter = 0;
    int nsd = 0;

    while(1)
    {            
        nsd = accept(sd, (void*)&cli, (socklen_t*)&addrlen);
        if(server_on == 0)
        {
            close(sd);
            break;
        }
        if(nsd == -1)
        {
            printf("Error: Connection Failed To Establish!\n");
        }
        else
        {
            pthread_create( &thread_id[counter] , NULL ,  login , (void*) &nsd);
            counter++;
        }
    }   
    
    for(unsigned long int i = 1; i < counter; i++)
    {
        pthread_join(thread_id[i], NULL); // waiting for all the active threads
    }
    shutdown(nsd, SHUT_RDWR);
    
    pthread_mutex_destroy(&mutex_lock1);
    pthread_mutex_destroy(&mutex_lock2);  
    return 0;
}  