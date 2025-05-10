#include "types.h"
#include "param.h"       // For NCPU, NOFILE
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"    // In order to avoid compilation error of proc
#include "proc.h"        // For struct proc
#include "petersonlock.h" // For struct petersonlock

#define MAX_PETERSON_LOCKS  15
struct petersonlock petersonlocks[MAX_PETERSON_LOCKS];

static void  internal_peterson_acquire(struct petersonlock *pl);
static void  internal_peterson_release(struct petersonlock *pl);

// Initialize the Peterson locks
// This function should be called once at the beginning of the program.
void init_petersonlock_arr(void) {
    struct petersonlock *pl;
    for (int i = 0; i < MAX_PETERSON_LOCKS; i++) {
        pl = &petersonlocks[i];
        pl->flag[0] = 0;
        pl->flag[1] = 0;
        pl->turn = 0;
        pl->initialized = 0;
        pl->lockId = -1;
        pl->pid = -1;
        pl->internal_lock = 0;
    }
}

int peterson_create(void) {
    struct petersonlock *pl;
    //Go over the petersonlocks array.
    for (int i = 0; i < MAX_PETERSON_LOCKS; i++) { 
        pl = &petersonlocks[i];        
        internal_peterson_acquire(pl);
        if (!pl->initialized) { //found an empty lock
            pl->flag[0] = 0;
            pl->flag[1] = 0;
            pl->turn = 0;
            pl->initialized = 1;
            pl->lockId = i;
            pl->pid = myproc()->pid;
            internal_peterson_release(pl);
            return petersonlocks[i].lockId;
        }
        internal_peterson_release(pl);
    }
    printf("[peterson_create] Error: No available lock\n");
    return -1; // No available lock
}

void static internal_peterson_acquire(struct petersonlock *pl) {
    push_off(); // disable interrupts to avoid deadlock.
    while (__sync_lock_test_and_set(&pl->internal_lock, 1)) {
      yield(); // let other processes run
    }
    __sync_synchronize(); // Memory barrier
  }
  
void static internal_peterson_release(struct petersonlock *pl) {
    __sync_synchronize(); // Memory barrier
    __sync_lock_release(&pl->internal_lock);
    pop_off(); // re-enable interrupts
  }


// Peterson algorithm for 2 processes.
int peterson_acquire(int lock_id, int role) {
    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS) {
        printf("[peterson_acquire] Error: Invalid lock ID\n");
        return -1; // Invalid lock ID
    }

    struct petersonlock *pl = &petersonlocks[lock_id];

    if (!pl->initialized) {
        printf("[peterson_acquire] Error: Lock not initialized\n");
        return -1; // Lock not initialized
    }

    pl->flag[role] = 1;
    pl->turn = 1 - role;

    // Busy-wait with Yield until it’s safe to enter critical section
    while (pl->flag[1 - role] && pl->turn == (1 - role)) {
        yield(); // Yield CPU to avoid wasteful spinning
    }

    return 0;
}

// Peterson algorithm for 2 processes.
int peterson_release(int lock_id, int role) {

    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS) {
        printf("[peterson_release] Error: Invalid lock ID\n");
        return -1; // Invalid lock ID
    }

    struct petersonlock *pl = &petersonlocks[lock_id];

    if (!pl->initialized) {
        printf("[peterson_release] Error: Lock not initialized\n");
        return -1; // Lock not initialized
    }

    pl->flag[role] = 0;

    return 0;
}


int peterson_destroy(int lock_id) {
    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS) {
        printf("[peterson_destroy] Error: Invalid lock ID\n");
        return -1; // Invalid lock ID
    }

    struct petersonlock *pl = &petersonlocks[lock_id]; //lock_id is the index in petersonlocks

    internal_peterson_acquire(pl);

    if (!pl->initialized) {
        internal_peterson_release(pl);
        printf("[peterson_destroy] Error: Lock not initialized\n");
        return -1; // Lock was not initialized
    }

    pl->initialized = 0;
    pl->pid = -1;

    internal_peterson_release(pl);

    return 0; // Successfully destroyed
}

