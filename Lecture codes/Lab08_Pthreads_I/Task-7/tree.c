#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Function executed by child threads created by first-level threads.
void *childThread(void *arg) {
    int childID = *(int *)arg;
    printf("    Child thread %d started.\n", childID);
    // Simulate some work here if desired.
    printf("    Child thread %d finished.\n", childID);
    pthread_exit(NULL);
}

// Function executed by first-level threads created by the main thread.
void *firstLevelThread(void *arg) {
    int firstLevelID = *(int *)arg;
    printf("First-level thread %d started.\n", firstLevelID);

    pthread_t childThreads[2];
    int childIDs[2];
    // Each first-level thread creates two child threads.
    for (int i = 0; i < 2; i++) {
        // Generate a unique id for the child thread (for demonstration).
        childIDs[i] = firstLevelID * 10 + i;
        if (pthread_create(&childThreads[i], NULL, childThread, &childIDs[i]) != 0) {
            perror("pthread_create failed for child thread");
            exit(1);
        }
        printf("First-level thread %d created child thread %d.\n", firstLevelID, childIDs[i]);
    }
    // Wait for both child threads to finish.
    for (int i = 0; i < 2; i++) {
        pthread_join(childThreads[i], NULL);
    }
    printf("First-level thread %d finished waiting for its child threads.\n", firstLevelID);
    pthread_exit(NULL);
}

int main() {
    printf("Main thread started.\n");

    pthread_t firstLevelThreads[2];
    int firstLevelIDs[2];
    // Main thread creates two first-level threads.
    for (int i = 0; i < 2; i++) {
        firstLevelIDs[i] = i + 1;  // Assign a unique id (1 and 2).
        if (pthread_create(&firstLevelThreads[i], NULL, firstLevelThread, &firstLevelIDs[i]) != 0) {
            perror("pthread_create failed for first-level thread");
            exit(1);
        }
        printf("Main thread created first-level thread %d.\n", firstLevelIDs[i]);
    }
    // Wait for both first-level threads to finish.
    for (int i = 0; i < 2; i++) {
        pthread_join(firstLevelThreads[i], NULL);
    }
    printf("Main thread finished waiting for first-level threads.\n");

    return 0;
}
