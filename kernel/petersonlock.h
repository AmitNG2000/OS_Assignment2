// Long-term locks for processes
struct petersonlock {
    uint flag[2];
    uint turn;
    
    // For debugging:
    int lockId;        // lock id = index
    int pid;           // Process holding lock
    int deleted = 1;       // 1 if lock is deleted
  };