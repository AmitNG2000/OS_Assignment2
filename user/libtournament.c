#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_PROCESSES 16
#define TOURNAMENT_PARENT -2

static int *lock_ids; // Array of lock ids

static int *roles_path; // Prosesse's path at the tree. Every childe will have it own copy.
static int *lock_ids_path; // Prosesse's path at the tree. Every childe will have it own copy.

static int L = -1; // Number of levels in the tree


///////////////////// Helper Functions Declaration /////////////////////

int tournament_is_valid_number_of_processes(int n);

int log2(int n);

void assign_path(int proc_index);

/////////////////////////////////////////////////////////////////////////

// Create a tournament tree (locs and proccess) with the given number of processes.
// Returns the child process its ID, -2 to the parent, and -1 on error.
int tournament_create(int processes) {

    if (!tournament_is_valid_number_of_processes(processes)) {
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
    for (int index = 0; index < processes; index++) {
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
    return TOURNAMENT_PARENT;
}


// Acquire the locks along the calling process's path, from the bottom to the root lock.
int tournament_acquire(void) {
    for (int l = 0; l < L; l++) {
        if (peterson_acquire(lock_ids_path[l], roles_path[l]) < 0) {
            printf("[tournament_acquire] Failed at level %d\n", l);
            return -1;
        }
    }
    return 0;
}

// Release the locks along the calling process's path, from the root to the bottom lock.
int tournament_release(void) {
    for (int l = L - 1; l >= 0; l--) {
        if (peterson_release(lock_ids_path[l], roles_path[l]) < 0) {
            printf("[tournament_release] Failed at level %d\n", l);
            return -1;
        }
    }
    return 0;
}

int tournament_destroy(void) {
    if (lock_ids == 0 || L == -1) {
        return -1; // Nothing to destroy or tournament not initialized
    }

    int num_locks = (1 << L) - 1; // same as: total processes - 1
    for (int i = 0; i < num_locks; i++) {
        peterson_destroy(lock_ids[i]);
    }

    free(lock_ids);
    lock_ids = 0;
    L = -1;

    return 0;
}


/////////////////////////// Helper Functions ///////////////////////////

int tournament_is_valid_number_of_processes(int n) {
    int is_power_of_2 = (n & (n - 1)) == 0;
    return (n > 0 && n <= MAX_PROCESSES && is_power_of_2);
}

int log2(int n) {
    int log = 0;
    while ((1 << log) < n)
        log++;
    return log;
}


/*
 * Executed only by the child process.
 * Assigns lock IDs and roles to climb the tournament tree.
 * Path consists of two arrays (size L): lock IDs and roles.
 * At each level l, acquires lock_ids_path[l] with role roles_path[l].
 */
void assign_path(int proc_index) {
    roles_path = malloc(L * sizeof(int));
    lock_ids_path = malloc(L * sizeof(int));

    int node = proc_index + (1 << L) - 1; // Leaf node index in binary tree

    for (int l = 0; l < L; l++) {
        int parent = (node - 1) / 2;
        roles_path[l] = (node % 2 == 0) ? 1 : 0;  // right child = role 1
        lock_ids_path[l] = lock_ids[parent];
        node = parent; // Move to parent node
    }
}

void print_path() {
    printf("Path: \n");
    for (int l = 0; l < L; l++) {
        printf("level: %d, lock id %d , role: %d\n", l, lock_ids_path[l], roles_path[l]);
    }
}

/////////////////////////////////////////////////////////////////////////