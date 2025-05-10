#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"


int main() {
    /*
    // Test the peterson_create function. Expected result: Not enough locks.
    for (int i = 0; i < 100; i++) {
        peterson_create();
    }
    */
    

    int lock_id = peterson_create();
    if (lock_id < 0) {
        printf("Failed to create lock\n");
        exit(1);
    }

    int fork_ret = fork();
    int role = fork_ret > 0 ? 0 : 1;

    for (int i = 0; i < 100; i++) {
        if (peterson_acquire(lock_id, role) < 0) {
            printf("Failed to acquire lock\n");
            exit(1);
        }

        // Critical section
        if (role == 0) {
            printf("Parent process in critical section\n");
        }
        else {
            printf("Child process in critical section\n");
        }
            
        if (peterson_release(lock_id, role) < 0) {
            printf("Failed to release lock\n");
            exit(1);
        }

        /* 
        // sleep for parent in order to allow child process to run and get not round-robin scheduling
        if (role == 0) { //prant process
                sleep(1); // Sleep to allow child process to run
            }
        */
        
    }

    if (fork_ret > 0) {
        wait(0);
        printf("Parent process destroying lock\n");
        if (peterson_destroy(lock_id) < 0) {
            printf("Failed to destroy lock\n");
            exit(1);
        }
    }

    return 0;
}