// Long-term locks for processes
struct petersonlock {
    int flag[2];
    int turn;
    int initialized;    // 1 if lock is deleted
    int internal_lock;          // 1 if lock is held
 
    
    // For debugging:
    int lockId;        // lock id = index
    int pid;           // Process holding lock
  };