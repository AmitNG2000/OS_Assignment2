#include "types.h"
#include "petersonlock.h"

#define numOfPetersons 15
struct petersonlock petersonlocks[numOfPetersons];
int petersonlock_count = 0;


int peterson_create(void) {
    struct petersonlock pl = (struct petersonlock)
    malloc(sizeof(struct petersonlock));
    for (int i = 0; i < numOfPetersons; i++) {
        petersonlocks[i].flag[0] = 0;
        petersonlocks[i].flag[1] = 0;
        petersonlocks[i].turn = 0;
        petersonlocks[i].pid = -1;
    }
    return 0;
}


static struct proc*
allocproc(void)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state == UNUSED) {
      goto found;
    } else {
      release(&p->lock);
    }
  }
  return 0;


int peterson_acquire(int lock_id, int role) {

}


int peterson_release(int lock_id, int role) {

}


int peterson_destroy(int lock_id) {

}
