//
// Created by René Zarwel on 21.03.16.
//

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS     2

pthread_t threads[NUM_THREADS];

void *threadPrint(void *arg)
{
    //Wait a random time between 1 and 5 secondes
    unsigned int sleepTime = (unsigned int)rand()%5 + 1;
    sleep(sleepTime);

    //Exit
    printf( "Thread %d: finished after %ds sleep ...exiting \n", (unsigned int)pthread_self(), sleepTime);
    pthread_exit(pthread_self());
}

int main ()
{
    //saves create and join return values for failures
    int rc;
    //index for for-loops
    int i;
    //attributes of the created threads
    pthread_attr_t attr;
    //returned status pointer of threads
    void *status;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //Init Random
    srand((unsigned int) time( NULL ));

    for( i=0; i < NUM_THREADS; i++ ){
        printf( "main() : creating thread %d  \n", i);
        rc = pthread_create(&threads[i], NULL, threadPrint, NULL );
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
        printf( "Main: completed thread id : %d  \n", (unsigned int)threads[i]);

        printf( "Exiting with status : %d  \n", rc);

        printf( "ID is same: %s  \n", (pthread_equal(status , threads[i])? "TRUE" : "FALSE"));

    }

    printf( "Main: program exiting.");

    pthread_exit(NULL);
}