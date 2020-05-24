#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *myThread(void *threadid) 
{
   sleep(1);
   long tid;
   tid = (long)threadid;
   printf("Thread ID: %ld\n", tid);
   return NULL;
}

int main()
{
    pthread_t thread[3]; // to create 3 threads, we need 3 thread ids
    for(int i = 0; i < 3; i++)
    {
        printf("Creating thread: %ld\n", thread[i]);
        pthread_create(&thread[i], NULL, myThread, (void *) (thread[i]));
        pthread_join(thread[i], NULL);
    }
    return 0;
}