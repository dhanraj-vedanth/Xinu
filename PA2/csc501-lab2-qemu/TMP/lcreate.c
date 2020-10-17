#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

/*
struct mylocker{

    int lstate;     // If Lock is free then lstate -> 1 else lstate -> 0
    int ltype;      // If it's a reader lock then ltype -> 1 else ltype -> 0
    int lhead;
    int ltail;
    int lread;     //Should not be crossing 1 if its a write lock I guess
    int lwrite;
    int owner; 
    int holders[NPROC]; //Keeps a tab of who is trying to access me! 
}
*/

int lcreate(){

    STATWORD ps;
    disable(ps);
    int retr = create_lock();
    if(retr == SYSERR){
        restore(ps);
        return SYSERR;
    }
    restore(ps);
    return retr;
}

int create_lock(){

    int i = 0;
    for(i;i<NLOCKS; i++){
        if (lok[i].lstate == LFREE){
            lok[i].lstate = LUSE;
            lok[i].owner = currpid;
            // printf("Creating this lock -> %d\n",i);
            return i;
        }
    }
    // printf("SYSERRRR\n");
    return SYSERR;
}