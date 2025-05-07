#include "types.h"
#include "petersonlock.h"
#include "defs.h"
#include "proc.h"

#define MAX_PETERSON_LOCKS  15
struct petersonlock petersonlocks[MAX_PETERSON_LOCKS];

int peterson_create(void) {
    for (int i = 0; i < MAX_PETERSON_LOCKS; i++) {
        
        internal_peterson_acquire(&petersonlocks[i]); //TODO
        if (!petersonlocks[i].initialized) { //found an empty lock
            petersonlocks[i].flag[0] = 0;
            petersonlocks[i].flag[1] = 0;
            petersonlocks[i].turn = 0;
            petersonlocks[i].initialized = 1;
            petersonlocks[i].lockId = i;
            petersonlocks[i].pid = myproc()->pid;
            internal_peterson_release(&petersonlocks[i]); //TODO
            return petersonlocks[i].lockId;
        }
    }
    return -1; // No available lock
}

void internal_peterson_acquire(struct petersonlock *pl) {
    push_off(); // disable interrupts to avoid deadlock.
    while (__sync_lock_test_and_set(&pl->internal_lock, 1)) {
      yield(); // let other processes run
    }
    __sync_synchronize(); // Memory barrier
  }
  
void internal_peterson_release(struct petersonlock *pl) {
    __sync_synchronize(); // Memory barrier
    __sync_lock_release(&pl->internal_lock);
  }



int peterson_acquire(int lock_id, int role) {
    if (lock_id < 0 || lock_id >= MAX_PETERSON_LOCKS)
        return -1; // Invalid lock ID

    struct petersonlock *pl = &petersonlocks[lock_id];
    internal_peterson_acquire(pl); // acquire internal lock

    if (!pl->initialized)
        return -1; // Lock not initialized

    pl->flag[role] = 1;
    pl->turn = role ^ 1;

    internal_peterson_acquire(pl); // done writing shared fields

    // Busy-wait until it’s safe to enter critical section
    while (pl->flag[role ^ 1] && pl->turn == (role ^ 1)) {
        yield(); // Yield CPU to avoid wasteful spinning
    }

    return 0;
}


int peterson_release(int lock_id, int role) {

}


int peterson_destroy(int lock_id) {

}
