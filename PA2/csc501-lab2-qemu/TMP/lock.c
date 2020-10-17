#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lock.h>
// struct mylocker{

//     int lstate;     // If Lock is free then lstate -> 1 else lstate -> 0
//     int ltype;      // If it's a reader lock then ltype -> 1 else ltype -> 0
//     int lhead;
//     int ltail;
//     int lread;     //Should not be crossing 1 if its a write lock I guess
//     int lwrite;
//     int time;
//     int owner;
//     // int lockers[NPROC]; //Keeps a tab of who is trying to access me! 
// }
/*
#define LFREE 1
#define LUSE  0

#define DELETED 1
#define READ 2
#define WRITE 3 
*/
int lock (int ldes, int type, int priority){
    // printf("Process [%d] is trying to access\n",currpid);
    STATWORD ps;
    int wait = 0;
    struct mylocker *lptr;
    lptr = &lok[ldes];
    // printf("Lock -> %d and state -> %d\n", ldes, lptr->lstate);
    disable(ps);
    if(ldes<0 || ldes >= NLOCKS || lok[ldes].lstate == LFREE){
        restore(ps);
        // printf("Dei inga dhaan da iruken echa\n");
        return SYSERR;
    }

    if (lptr->lread == 0 && lptr->lwrite == 0){
        wait = 0;
    }
    else if(lptr->lread > 0 && lptr->lwrite == 0){
        if (type == READ){
    //  Allow only if there is no writer lock with a higher prio is my understanding 
    // wait = 0?
        int head = lptr->lhead;
        int tail = q[lptr->ltail].qprev;
        for(tail; tail != head && q[tail].qkey > priority; tail = q[tail].qprev){
            if (proctab[tail].lockstate[ldes] == WRITE){
                wait = 1;
                }
            }
        }
        else{
            wait = 1;
        }
    }
    else{
        // Now Number of writers is not equal to zero. 
        wait = 1;
    }
    // printf("Wait value --> %d\n",wait);
    if (wait == 0){
        // Go ahead and give the asking process the lock I guess and Increment according to the type! 
        if (type == READ){
            // printf("Reader about to acquire\n");
            
            lptr->lread+= 1;
        }
        else if (type == WRITE){
            // printf("Writer about to acquire\n");
            lptr->lwrite += 1;
        }
        struct pentry *pptr;
        lptr->lstate = LUSE;
        lptr->ltype = type;
        lptr->holders[currpid] = 1;
        pptr = &proctab[currpid];
        pptr->lockstate[ldes] = type;
        restore(ps);
        return (OK);

    }
    if (wait == 1){
        // printf("Pushed to the QUEUE \n");
        //push current process into wait queue of the semaphore/lock
        struct pentry *pptr;
        pptr = &proctab[currpid];
        int highest_prio = 0;
        if (type == READ){
            // int highest_prio = find_highest(WRITE,ldes);
            // printf("LOCK ACQUIRINGGG --> %d[highest write]\n", q[highest_prio].qkey);
            // if (priority > q[highest_prio].qkey && highest_prio != 0){
            //     printf("WHOHOO SUCCESS\n");
            //     struct pentry *pptr;
            //     lptr->lread += 1;
            //     lptr->lstate = LUSE;
            //     lptr->ltype = type;
            //     lptr->holders[currpid] = 1;
            //     pptr = &proctab[currpid];
            //     pptr->lockstate[ldes] = type;
            //     pptr->wait_times = ctr1000;
            //     restore(ps);
            //     return (OK);
            // }
            // else{
                // printf("[%d] Reader getting pushed in\n",currpid);
                pptr->blkid = ldes; //Has info about which semaphore it is waiting on 
                pptr->pstate = PRLWAIT;
                pptr->lockstate[ldes] = type;
                pptr->wait_times = ctr1000;
                pptr->blkid = ldes;
                insert(currpid, lptr->lhead, priority);
                check_prio_inv(currpid,ldes);
                resched();
                restore(ps);
                return pptr->pwaitret;
                // return OK;
            // }
        }
        if (type == WRITE){
            // printf("[%d] Writer getting pushed in\n",currpid);
            pptr->blkid = ldes; //Has info about which semaphore it is waiting on 
            pptr->pstate = PRLWAIT;
            pptr->lockstate[ldes] = type;
            pptr->wait_times = ctr1000;
            pptr->blkid = ldes;
            insert(currpid, lptr->lhead, priority);
            check_prio_inv(currpid,ldes);
            resched();
            restore(ps);
            return pptr->pwaitret;
            // return OK;
        }
        // print_queue(ldes);
    }
 }


find_highest(int type, int ldes){
    // printf("Queue details for type[%d]\n",type);
    int max_prio = 0;
    int proc = 0;
    struct mylocker *lptr;
    lptr = &lok[ldes];
    int head = lptr->lhead;
    int tail = q[lptr->ltail].qprev;
    for (tail;tail != head; tail = q[tail].qprev){
        // printf("%d --> %d\n", tail,q[tail].qkey);
        // printf("process holding state -> %d\n",proctab[tail].lockstate[ldes]); 
        if (proctab[tail].lockstate[ldes] == type){
            if (q[tail].qkey > max_prio){
                max_prio = q[tail].qkey;
                proc = tail;
            }
        }
    }
    return proc;
    }

void print_queue(int ldes){
    printf("*********** QUEUE DETAILS FOR LOCK - %d ***************\n", ldes);
    struct mylocker *lptr;
    lptr = &lok[ldes];
    int head = lptr->lhead;
    int tail = q[lptr->ltail].qprev;
    for (tail;tail != head; tail = q[tail].qprev){
        printf("Process[%d] -> Prio[%d] | type->%d \n", tail,q[tail].qkey,proctab[tail].lockstate[ldes]);
    }
    printf("****************** DETAILS END HERE **********************\n");
}

void print_holders(int ldes){
    printf("***************HOLDERS FOR THE LOCK - %d *****************\n",ldes);
    struct mylocker *lptr;
    lptr = &lok[ldes];
    int i = 0;
    for(i;i<NPROC;i++){
        if (lptr->holders[i] == 1){
            printf("%d ",i);
        }
    }
    printf("\n");
    printf("****************** DETAILS END HERE **********************\n");
    
}


int prio_inv_highest(int ldes){
    // print_holders(ldes);
    int max_prio = 0; 
    int max_proc = 0;
    struct pentry *pptr; 
    struct mylocker *lptr;
    lptr = &lok[ldes];
    int i = 0;
    for(i;i<NPROC;i++){
        if (lptr->holders[i] == 1){
            pptr = &proctab[i];
            if (pptr->lockstate[ldes] == READ || pptr->lockstate[ldes] == WRITE){
                int prio = pptr->pprio;
                int inhp = pptr->pinh;
                int final_res = 0;
                if (prio > inhp){
                    final_res = prio;
                }
                else{
                    final_res = inhp;
                }
                if (final_res > max_prio){
                    max_prio = final_res;
                    max_proc = i;
                }
                // if (pptr->pinh == 0){
                //     printf("Before here\n");
                //     if (pptr->pprio > max_prio){
                //         printf("here\n");
                //         max_prio = pptr->pprio;
                //         max_proc = i;
                //     }
                // /* NEED TO HANDLE FOR WHAT IF INHERITED PRIO IS THERE! */
                // if (pptr->pinh !=0){
                //     printf("Before here pt2\n");
                //     if (pptr->pinh > max_prio){
                //         printf("here pt2\n");
                //         max_prio = pptr->pinh;
                //         max_proc = i;
                //     }
                }

                }
            }
    //     }
    // }
    // printf("final max_prio[%d] and max_proc[%d]\n",max_prio,max_proc);
    return max_proc;
}


void check_prio_inv(int pid, int ldes){
    /*
    Need to find the highest prio of the processes currently holding ldes
    compare with the process of this pid
    if its greater, then its fine I guess
    If it is not greater, Need to bump up the prio 
    */
    // printf("\n    ******PRIO INV CHECK*******  \n");
    struct pentry *pptr;
    pptr = &proctab[pid];
    struct mylocker *lptr;
    lptr = &lok[ldes];
    int highest_prio = -1;
    highest_prio = prio_inv_highest(ldes);
    // printf("This is the culprit -> %d[%d]/[%d]\n", highest_prio, proctab[highest_prio].pprio,proctab[highest_prio].pinh);
    // printf("Prio inv checked for -> %d[%d]/[%d]\n", pid, proctab[pid].pprio,proctab[pid].pinh);
    if (proctab[highest_prio].blkid != -1 && highest_prio != 0){
        /*
        WHAT TO DO IF THIS PROCESS IS IN TURN WAITING IN SOMEOTHER QUEUE
        GOTTA INHERIT AGAIN
        */
        // printf("RECURSIVE CALLLL\n%d is waiting on --> %d\n", highest_prio,proctab[highest_prio].blkid);
        check_prio_inv(highest_prio,proctab[highest_prio].blkid);
    }

    if (highest_prio == -1){
        if (proctab[highest_prio].pinh != 0){
            proctab[highest_prio].pprio = proctab[highest_prio].pinh;
        }
        else{
            proctab[highest_prio].pinh = proctab[highest_prio].pprio;
        }
    }
    else{
            if(proctab[highest_prio].pprio > proctab[pid].pprio ){
                // Nothign to do, let it run
            }
            else{
                // The highest prio of all the holders available is lesser than this curr prio
                // So Loop through the list of holders and update their pinh/pprio as needed
                // printf("Prio inverstion happening\n");
                int i = 0;
                for(i;i<NPROC;i++){
                    if (lptr->holders[i] == 1){
                        if (proctab[i].pinh != 0){
                            proctab[i].pinh = proctab[i].pprio;
                            proctab[i].pprio = proctab[pid].pprio;
                        }
                        else{
                            proctab[i].pinh = proctab[i].pprio;
                            proctab[i].pprio = proctab[pid].pprio;
                        }
                // if (proctab[highest_prio].pinh == 0){
                //     proctab[pid].pinh = proctab[highest_prio].pprio;
                //     proctab[pid].pprio = proctab[pid].pinh; 
                // }
                // // else if (proctab[pid].pinh != 0){
                // //     proctab[pid].pinh = proctab[highest_prio].pprio;
                // //     proctab[pid].pprio = 
                // // }
                // else{
                //     proctab[pid].pinh = proctab[highest_prio].pinh;
                //     proctab[pid].pprio = proctab[pid].pinh;
                // }
            printf("\nChanged prio of proc[%d] pprio:%d pinh: %d\n\n",i,proctab[i].pprio,proctab[i].pinh);   
            if (proctab[i].pstate == PRREADY){
                dequeue(i);
                insert(i,rdyhead,proctab[i].pprio);
            }
            }
        }

    }
}
}

