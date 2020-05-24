#define LEN 1024
#define ACC 5

typedef struct account
{
    unsigned int this_id;
    unsigned int id[ACC];
    char password[LEN];
    float balance;
}   account_t;

#define SIZE sizeof(account_t)

void itoa(int value, char* str, int base);