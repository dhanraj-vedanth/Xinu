#ifndef LOCKS
#define LOCKS

#ifndef NLOCKS
#define NLOCKS 50
#endif

#define LFREE 1
#define LUSE  0

// #define DELETED 1
#define READ 2
#define WRITE 3 
// extern int count;
extern unsigned long ctr1000;


struct mylocker{

    int lstate;     // If Lock is free then lstate -> 1 else lstate -> 0
    int ltype;      // If it's a reader lock then ltype -> 1 else ltype -> 0
    int lhead;
    int ltail;
    int lread;     //Should not be crossing 1 if its a write lock I guess
    int lwrite;
    int owner; 
    int holders[NPROC]; //Keeps a tab of currently has access to me! 
};

extern struct mylocker lok[NLOCKS];
void linit();
int find_highest(int,int);
int waitqcheck(int);
int lock (int, int, int);
int prio_inv_highest(int);
void check_prio_inv(int,int);
int releaseall (int, int , ...);
int waitqcheck(int);
int get_highest_prio(int);
void update_prio(int);
void print_queue(int);
void print_holders(int);
void release_kill_chprio_logic(int);


#endif