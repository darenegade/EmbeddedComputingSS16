//
// Created by René Zarwel on 21.03.16.
//

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS     2

pthread_t threads[NUM_THREADS];

void *threadPrint(void *t)
{
    long tid;

    tid = (long)t;

    sleep(tid + 1);
    printf( "Thread %d finished ...exiting \n", tid);
    pthread_exit(pthread_self());
}

int main ()
{
    int rc;
    int i;
    pthread_attr_t attr;
    void *status;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for( i=0; i < NUM_THREADS; i++ ){
        printf( "main() : creating thread %d  \n", i);
        rc = pthread_create(&threads[i], NULL, threadPrint, (void *)i );
        if (rc){
            printf( "Error:unable to create thread, %d  \n", rc);

            exit(-1);
        }
    }

    // free attribute and wait for the other threads
    pthread_attr_destroy(&attr);
    for( i=0; i < NUM_THREADS; i++ ){
        rc = pthread_join(threads[i], &status);
        if (rc){
            printf( "Error:unable to join, %d  \n", rc);

            exit(-1);
        }
        printf( "Main: completed thread id : %d  \n", i);

        printf( "Exiting with status : %d  \n", rc);

        printf( "ID is same: %s  \n", ((status == threads[i])? "TRUE" : "FALSE"));

    }

    printf( "Main: program exiting.");

    pthread_exit(NULL);
}