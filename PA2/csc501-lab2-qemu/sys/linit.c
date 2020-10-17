#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

struct mylocker lok[NLOCKS];

void linit(){
    struct mylocker *lptr;
    int i;
    for(i=0; i< NLOCKS; i++)
    {
        lptr = &lok[i];
        lptr->lstate = LFREE;
        // lptr->ltype 
        lptr->ltail = 1 + (lptr->lhead=newqueue());
        lptr->lread = 0;
        lptr->lwrite = 0;
        int pid = 0;
        for(pid; pid<NPROC; pid++){
            lptr->holders[pid] = 0;
            }
        
    }
}
