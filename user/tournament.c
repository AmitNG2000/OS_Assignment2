#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define TOURNAMENT_PARENT -2  // like in libtournament.c


int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Number of processes is reqired\n");
        exit(1);
    }
    int num_processes = atoi(argv[1]);
    int tournament_id = tournament_create(num_processes);

    // Child
    if (tournament_id >= 0) {
        if (tournament_acquire()<0) exit(1);
        printf("Child process with pid: %d and tournament_id: %d, is in critical section\n", getpid(), tournament_id);
        tournament_print_path();
        if (tournament_release() < 0) exit(1);
        // It is unnecessary to clean up children’s data, as they require it during execution and the OS delete it upon the process' termination.
        exit(0); 
    }
    // Pranet
    if (tournament_id == TOURNAMENT_PARENT) {
        // Wait for all child processes to finish
        for (int i = 0; i < num_processes; i++) {
            wait(0); 
        }
        if (tournament_destroy() < 0) exit(1);
        exit(0);
    }
    // Error
    if (tournament_id == -1) {
        printf("Failed to create tournament\n");
        tournament_destroy();
        exit(1);
    }

    return -1; // This line is unreachable, but added to avoid compiler warnings
}

