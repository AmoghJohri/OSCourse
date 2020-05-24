#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


void *thread_1(void *vargp)
{
    sleep(1);
    printf("Thread 1 is executed!\n");
    return NULL;
}
void *thread_2(void *vargp)
{
    sleep(1);
    printf("Thread 2 is executed!\n");
    return NULL;
}
void *thread_3(void *vargp)
{
    sleep(1);
    printf("Thread 3 is executed!\n");
    sleep(1);
    return NULL;
}

int main()
{
    pthread_t thread_id_1,thread_id_2,thread_id_3;
    printf("This is the calling thread!\n");
    pthread_create(&thread_id_1, NULL, thread_1, NULL);
    pthread_join(thread_id_1, NULL);
    pthread_create(&thread_id_2, NULL, thread_2, NULL);
    pthread_join(thread_id_2, NULL);
    pthread_create(&thread_id_3, NULL, thread_3, NULL);
    pthread_join(thread_id_3, NULL);
    printf("Back to the calling thread!\n");
    return 0;
}

