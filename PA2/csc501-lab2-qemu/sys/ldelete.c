#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lock.h>
/*
#define LFREE 1
#define LUSE  0

#define READ 1
#define WRITE 0 

struct mylocker{
    int lstate;     // If Lock is free then lstate -> 1 else lstate -> 0
    int ltype;      // If it's a reader lock then ltype -> 1 else ltype -> 0
    int lhead;
    int ltail;
    int lread;     //Should not be crossing 1 if its a write lock I guess
    int lwrite;
    int time;
    int owner;
    // int lockers[NPROC]; //Keeps a tab of who is trying to access me! 
}
*/
int ldelete (int lockdescriptor){

    STATWORD ps;
    // printf("Deleting this lockdes -> %d; my state rn -> %d\n",lockdescriptor, lok[lockdescriptor].lstate);

    struct mylocker *lptr;
    lptr = &lok[lockdescriptor];
    disable(ps);
    if (lockdescriptor <0 || lockdescriptor >=NLOCKS || lptr->lstate == LFREE ){
        restore(ps);
        return SYSERR;
    }
    // struct mylocker *lptr;
    struct pentry *pptr;
    lptr->lstate = LFREE;
    lptr->lread = 0;
    lptr->lwrite = 0;
    // lptr->time = 0;
    int now = lptr->lhead;
    int pid;
    // printf("Changed stuffs lockdes -> %d; my state rn -> %d\n",lockdescriptor, lok[lockdescriptor].lstate);
    // if(nonempty(lptr->lhead)) {
    //     while(now != lptr->ltail){
    //         int cur_pid = getlast(lptr->ltail);
    //         pptr = &proctab[cur_pid];
    //         pptr->lockstate[now] = DELETED;
    //         pptr->pstate = PRREADY;
    //         insert(cur_pid,rdyhead, pptr->pprio);
    //         now = q[now].qnext;
    //     }
	// }
  int i = 0;
  for (i; i<NPROC; i++){
    if (lptr->holders[i] == 1){
      // Damn I'm holding this lock but it's deleted lol
      proctab[i].pwaitret = DELETED;
      proctab[i].lockstate[lockdescriptor] = DELETED;
    }
  }
  if (nonempty(lptr->lhead)) {
  while( (pid=getfirst(lptr->lhead)) != EMPTY)
    {
      proctab[pid].pwaitret = DELETED;
      proctab[pid].lockstate[lockdescriptor] = DELETED;
      ready(pid,RESCHNO);
    }
  resched();
	}
    restore(ps);
    return OK;


}