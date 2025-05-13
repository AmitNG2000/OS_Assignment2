#include "types.h"
#include "user.h"

// Global variables for the tournament lock
static int *lock_tree;
static int num_processes;
static int tree_size;

// Function to create the tournament lock
int tournament_create(int processes) {
    //The expression (processes & (processes - 1)) != 0 is a  bitwise operation used to check if a number is a power of 2.
    if (processes <= 0 || processes > 16 || (processes & (processes - 1)) != 0) {
        return -1; // Invalid number of processes (must be a power of 2 and <= 16)
    }

    num_processes = processes;
    tree_size = 2 * num_processes - 1; // leaf nodes and the internal nodes

    lock_tree = (int *)malloc(tree_size * sizeof(int));
    if (lock_tree == 0) {
        return -1; // Memory allocation failed
    }

    for (int i = 0; i < tree_size; i++) {
        if (peterson_create()<0) {
            free(lock_tree);
            return -1; // Failed to create peterson lock
        }
    }

    return id; // TODO
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