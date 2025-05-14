#include "user/user.h"
#include "kernel/types.h"

#define MAX_PROCESSES 16
#define PARENT_SYMBOLE (MAX_PROCESSES + 1)

static int *lock_ids; // Array of lock ids

static int *roles_path; // Prosesse's path at the tree
static int *lock_ids_path; // Prosesse's path at the tree

static int L = -1; // Number of levels in the tree


/////////////////////////// Helper Functions ///////////////////////////

int is_power_of_2(int n) {
    return (n & (n - 1)) == 0;
}

int log2(int n) {
    int log = 0;
    while ((1 << log) < n) {
        log++;
    }
    return log;
}

/////////////////////////////////////////////////////////////////////////


int tournament_create(int processes) {

    if (!is_power_of_2(processes) || processes > MAX_PROCESSES || processes <= 0 ) {
        printf("[peterson_lock_test] Error: Number of processes must be a power of 2, positive and less than or equal to %d\n", MAX_PROCESSES);
        return -1;
    }
    
    // Initialize data arrays
    int num_locks = processes - 1;
    lock_ids = malloc(num_locks * sizeof(int));

    // Create all locks needed for the tree
    int lock_id;
    for (int i = 0; i < num_locks; i++) {
        lock_id = peterson_create();
        if (lock_id < 0) {
            // Clean up previously created locks
            for (int j = 0; j < i; j++) {
                peterson_destroy(lock_ids[j]);
            }
            free(lock_ids);
            return -1;
        }
        lock_ids[i] = lock_id;
    }

    // Init amount of levels in the tree
    L = log2(processes);

    // Fork processes
    int pid;
    int index;
    for (index = 0; index < processes; index++) {
        pid = fork();
        //error
        if (pid < 0)
            return -1; // Fork failed, do not clean up
        // Parent
        if (pid > 0)
            continue;
        //child
        if (pid == 0) {
            assign_path(index);
            return index;
        }  
    }

    // Only parent process gets here
    for (int i = 0; i < processes; i++) {
        wait(0); // Wait for all child processes to finish
    }
    return PARENT_SYMBOLE;
}


void assign_path(int proc_index) {
    int current_role = -1;
    int current_lock_index = -1;
    int current_lock_id = -1;
    roles_path = malloc(L * sizeof(int));
    lock_ids_path = malloc(L * sizeof(int));

    for (int l = 0; l < L; l++) {

        current_role = (proc_index & (1 << (L - l - 1))) >> (L - l - 1);
        roles_path[l]= current_role;


        current_lock_index =  proc_index >> (L - l);
        current_lock_id = lock_ids[current_lock_index];
        lock_ids_path[l] = current_lock_id;
    }
}

///////////////////////// co pilot from here //////////////////////

// Function to acquire the tournament lock
int tournament_acquire(void) {
    int pid = getpid();
    int index = tree_size - num_processes + pid;

    while (index > 0) {
        int parent = (index - 1) / 2;

        while (xchg(&lock_tree[parent], 1) != 0) {
            // Spin until the lock is acquired
        }

        index = parent;
    }

    return 0; // Lock acquired
}

// Function to release the tournament lock
int tournament_release(void) {
    int pid = getpid();
    int index = tree_size - num_processes + pid;

    while (index > 0) {
        int parent = (index - 1) / 2;

        lock_tree[parent] = 0; // Release the lock
        index = parent;
    }

    return 0; // Lock released
}