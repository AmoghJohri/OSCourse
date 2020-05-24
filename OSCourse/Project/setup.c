/*
The is just to setup the initial database.
*/

#include <fcntl.h>  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/stat.h>  
#include <sys/types.h>  
 
#define LEN 1024 // length of the password
#define ACC 5 // maximum number of ids permitted in a joint account
 
typedef struct account // this is used to store all the information regarding an account
{
    unsigned int this_id;
    unsigned int id[ACC];
    char password[LEN];
    float balance;
}   account_t;

#define SIZE sizeof(account_t) // size of the account datastructure

void strreverse(char* begin, char* end) { // function to reverse a string
	char aux;
	while(end>begin)
		aux=*end, *end--=*begin, *begin++=aux;
}
	
void itoa(int value, char* str, int base) { // function to get the string for an integer
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

int print_id(unsigned int* arr)
{
  int counter = 0;
  if(*(arr + counter) == 1) // this is the administrator's account
    return 1;
  printf("Ids attaced to this account are: ");
  while(*(arr + counter) != 0)
  {
    if(counter == 0) printf("%d",*(arr + counter));
    else printf(", %d",*(arr + counter));
    counter ++;
  }
  if(counter > 1)
    printf("(This is a joint account)\n");
  else
    printf("(This is a single account)\n");
  return 0;
}

void print_account_details(void) // prints all the account details
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
  int num_of_accounts = pointer->balance - 1;
  for(int i = 0; i < num_of_accounts; i++)
  {
      read(fd, pointer, SIZE);
      printf("++++++++++++++++++++++++++++\n");
      if(print_id(pointer->id) == 1)
        continue ;
      printf("Password: %s\n",pointer->password);
      printf("Balance: %f\n", pointer->balance);
      printf("++++++++++++++++++++++++++++\n");
  }
  free(pointer);
  close(fd);
}

 
int main() {
    int fd;
    account_t admin_account = {0, {0}, "password", 21.}; // the balance field refers to the total number of structs in the database
    admin_account.id[0] = 1;


    fd = open("pds.bin", O_WRONLY|O_CREAT|O_TRUNC, 0666); /* Open the file for writing */
    if (fd == -1) { /* In the case of error, open returns -1 ! */
      printf("Error: Database Failed To Open!\n");
      exit(1);
    }
    if(write(fd, &admin_account, SIZE) != SIZE) 
    {
        printf("Error: Failed To Write!\n"); 
        exit(1);
    }

    printf("Adding 10 dummy singe-user accounts...\n");
    account_t* dummy_account = (account_t*)calloc(1, sizeof(account_t));

    for(int i = 1; i < 11; i++)
    {   
        itoa(i+1, dummy_account->password, 10);
        dummy_account->id[0] = i+1;
        for(int j = 1; j < ACC; j++)
          dummy_account->id[j] = 0;
        dummy_account->balance = 0.0;

        if(write(fd, dummy_account, SIZE) != SIZE)
        {
        printf("Error: Failed To Write!\n"); 
        free(dummy_account);
        exit(1);
        }
    }
    printf("Adding 10 dummy joint-user accounts...\n"); // all with just two users
    for(int i = 11; i < 30; i = i + 2)
    {
        itoa(i+1, dummy_account->password, 10);
        dummy_account->id[0] = i+1;
        dummy_account->id[1] = i+2;
        for(int j = 2; j < ACC; j++)
          dummy_account->id[j] = 0;
        dummy_account->balance = 0.0;

        if(write(fd, dummy_account, SIZE) != SIZE)
        {
          printf("Error: Failed To Write!\n"); 
          free(dummy_account);
          exit(1);
        }
    }
    close(fd);

    print_account_details();

      
    return 0;
}