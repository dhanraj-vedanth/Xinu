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

// DO STUFFS HERE!!
/*
Check through the wait queue to see what the prio of the highest reader and writer is 
if reader has higher prio than that of writer:
1. Release this guy and all the other readers that are before this highest prio writer
2. If no writer just release them all
if writer has higher:
1. Check for that 0.4 time something 
2. Else just give it to writer I guess 
*/
int releaseall (int numlocks, int ldes1, ...){

    STATWORD ps;
    disable(ps);
    int retr = OK;
    struct pentry *pptr;
    pptr = &proctab[currpid];
    struct mylocker *lptr;
    unsigned long *a = (unsigned long)&ldes1;
    // lptr = &lock[ldes];
    int i = 0;
    for(i; i<numlocks;i++){
        int ldes;
        ldes = *a++;
        // printf("Release called for %d by proc %d\n",ldes,currpid);
        // print_queue(ldes);
        if(ldes <0 || ldes >=50){
            retr = SYSERR;
            continue;
        }
        lptr = &lok[ldes]; 
        if (lptr->holders[currpid] != 1){
            retr = SYSERR;
            continue;
        }
        if (pptr->lockstate[ldes] == READ || pptr->lockstate[ldes] == WRITE){
            int holding = pptr->lockstate[ldes]; //Am I a reader or writer
            // Need to think about the deleted case tho.
            pptr->lockstate[ldes] = 100;
            if (holding == READ){
                // printf("Reader Release\n");
                lptr->lread -= 1;
                lptr->holders[currpid] = -1;
            }
            else if (holding == WRITE){
                // printf("Writer Release\n");
                lptr->lwrite -= 1;
                lptr->holders[currpid] = -1;
                
            }
            int flag = waitqcheck(ldes);
            if (flag == 0)
            {
                // printf("No one in the queue\n");
                // if (lptr->lread == 0 && lptr->lwrite == 0){
                //     lptr->lstate = LFREE;
                // }
                // lptr->lread = 0;
                // lptr->lwrite = 0;
                // lptr->time = 0;
            }
            else{
                // printf("Gotta find someone to give the lock to!\n");
                int highest_read = find_highest(2,ldes);
                int highest_write = find_highest(3,ldes);
                int hr_prio = q[highest_read].qkey;
                int hw_prio = q[highest_write].qkey;
                // printf("Highest read[%d] --> %d and highest write[%d] --> %d\n",highest_read,q[highest_read].qkey,highest_write,q[highest_write].qkey);
                // printf("reads: %d writes: %d\n", lptr->lread, lptr->lwrite);
                // printf("hw_prio[%d] hr_prio[%d]\n",hw_prio,hr_prio);
                if (hr_prio > hw_prio && lptr->lwrite ==0){
                    // printf("case #1 [Pushing readers into the queue]\n");
                    // Don't have to worry about anything, go ahead and start giving the processes the locks until we hit a WRITE type
                    int head = lptr->lhead;
                    int tail = q[lptr->ltail].qprev;
                    // for(tail; tail != head ; tail = q[tail].qprev){
                    // print_queue(ldes);
                    while(tail!=head){
                        // printf("TOUCHING -> %d\n",tail);
                        if (tail == -1 || tail == 0){
                            break;
                        }
                        if (proctab[tail].lockstate[ldes] == READ){
                            // printf("Inserting %d back [%d]\n",tail,q[tail].qkey);
                            lptr->lread += 1;
                            lptr->holders[tail] = 1;
                            int val = dequeue(tail);
                            proctab[tail].pstate = PRREADY;
                            proctab[tail].blkid = -1;
                            if (proctab[tail].lockstate[ldes] == DELETED || lok[ldes].lstate == LFREE){
                                retr = SYSERR;
                            }
                            insert(tail,rdyhead,proctab[tail].pprio);
                            tail = q[lptr->ltail].qprev;
                        }
                        else if (proctab[tail].lockstate[ldes] == WRITE)
                            break;
                    }
                }
                else if (hw_prio > hr_prio && lptr->lread == 0){
                    // printf("case #2[Pushing Writer into ready queue]\n");
                    lptr->lwrite += 1;
                    int head = lptr->lhead;
                    int tail = q[lptr->ltail].qprev;
                    // printf("Inserting %d back [%d]\n",tail,q[tail].qkey);
                    int val = dequeue(tail);
                    lptr->holders[tail] = 1;
                    proctab[tail].pstate = PRREADY;
                    proctab[tail].blkid = -1;
                    if (proctab[tail].lockstate[ldes] == DELETED || lok[ldes].lstate == LFREE){
                        retr = SYSERR;
                    }
                    insert(tail,rdyhead,proctab[tail].pprio);

                }
                else if (hr_prio == hw_prio && (lptr->lread == 0 || lptr->lwrite == 0)){
                    // printf("Case #3 [Reader and writer has equal max prio]\n"); 
                    /*
                    Will have to calculate the wait times of both of these 
                    If the wait time of 
                    */
                    int reader_wait = 0;

                    int writer_wait = 0;
                    reader_wait = ctr1000 - proctab[highest_read].wait_times;
                    writer_wait = ctr1000 - proctab[highest_write].wait_times;
                    int compare;
                    if (writer_wait > reader_wait){
                        compare = writer_wait - reader_wait;
                    }
                    else{
                        compare = reader_wait - writer_wait;
                    }
                    if (compare < 400){ 
                        // Give reader preference right?
                        if (lptr->lwrite == 0){
                            lptr->lread += 1;
                            lptr->holders[highest_read] = 1;
                            int pid = dequeue(highest_read);
                            proctab[highest_read].pstate = PRREADY;
                            insert(highest_read,rdyhead,proctab[highest_read].pprio);
                            proctab[highest_read].blkid = -1;
                            if (proctab[highest_read].lockstate[ldes] == DELETED || lok[ldes].lstate == LFREE){
                                retr = SYSERR;
                            }
                        }
                        restore(ps);
                        return (OK);
                    }
                    else{
                        if (lptr->lread == 0){
                            lptr->lwrite += 1;
                            lptr->holders[highest_write] = 1;
                            int pid = dequeue(highest_write);
                            proctab[highest_write].pstate = PRREADY;
                            insert(highest_write,rdyhead,proctab[highest_write].pprio);
                            proctab[highest_write].blkid = -1;
                            if (proctab[highest_write].lockstate[ldes] == DELETED || lok[ldes].lstate == LFREE){
                                retr = SYSERR;
                            }
                        }
                        restore(ps); 
                        return(OK);
                    }
                }
            }
        }
        else if (pptr->lockstate[ldes] == DELETED){
            // printf("DELETED CASE!\n");
            retr = SYSERR;  //needed?
            // What if I'm trying to release a DELETED lock? 
            // In that case I have to release all the processes in the wait queue of this lock
            // Since they are literally just waiting like an idiot
            int flag = waitqcheck(ldes);
            if (flag == 0){
                 if (lptr->lread == 0 && lptr->lwrite == 0){
                    lptr->lstate = LFREE;
                }
            }
            else{
                int head = lptr->lhead;
                int tail = q[lptr->ltail].qprev;
                while(tail!=head){
                    // printf("Process %d -> prio[%d] inhp[%d]\n", tail,proctab[tail].pprio,proctab[tail].pinh);
                    if (tail == -1){
                        break;
                    }
                    if (tail >= 50){
                        tail = q[tail].qprev;
                        continue;
                    }
                    insert(tail,rdyhead,proctab[tail].pprio);
                    tail = q[lptr->ltail].qprev;
                }
            lptr->lstate = LFREE;
        }
        }
    }
    restore(ps);
    // if(proctab[currpid].blkid != -1){
    //     printf("calling kill/releaseall logic for pid -> %d\n",currpid);
    //     printf("I'm waiting in -> %d\n",proctab[currpid].blkid);
    //     sleep(50);
	// 	release_kill_chprio_logic(currpid);
	// }
    update_prio(currpid);
    // resched(); //NEEDED?
    return retr;
}

int waitqcheck(int ldes){
    int retr = 0;
    struct mylocker *lptr;
    lptr = &lok[ldes]; 
    // Below satisfies? Implies that the wait queue for this lock is empty. Can release it and make it free
    // Someother process is free to grab it now
    // if (q[lptr->lhead].qnext != q[lptr->ltail].qprev){
    //     retr = 1;
    // }
    if(nonempty(lptr->lhead)){
        retr = 1;
    }
    return retr;
}

int get_highest_prio(int ldes){
    int retr_prio = 0;
    int max_prio = 0;
    struct mylocker *lptr;
    lptr = &lok[ldes];
    int head = lptr->lhead;
    int tail = q[lptr->ltail].qprev;
    while(tail!=head){
        // printf("Process %d -> prio[%d] inhp[%d]\n", tail,proctab[tail].pprio,proctab[tail].pinh);
        if (tail == -1){
            break;
        }
        if (tail >= 50){
            tail = q[tail].qprev;
            continue;
        }
        if (proctab[tail].pprio > max_prio){
            // printf("Changes\n");
            max_prio = proctab[tail].pprio;
        }
        // int inhp = proctab[tail].pinh;
        // int realp = proctab[tail].pprio;
        // if (inhp >= realp){
        //     if (inhp > max_prio){
        //         max_prio = inhp;
        //         retr_prio = tail;
        //     }
        // else{
        //     printf("HERE!!!\n");
        //     if (realp > max_prio){
        //         max_prio = realp;
        //         retr_prio = tail;
        //         }
        //     }
        // }
        tail = q[tail].qprev;
    }
    // for(tail; tail != head ; tail = q[tail].qprev){
    //     int inhp = proctab[tail].pinh;
    //     int realp = proctab[tail].pprio;
    //     if (inhp >= realp){
    //         if (inhp > max_prio){
    //             max_prio = inhp;
    //             retr_prio = tail;
    //         }
    //     else{
    //         if (realp > max_prio){
    //             max_prio = realp;
    //             retr_prio = tail;
    //         }
    //     }
    //     }
    // }
    // printf("Finally -> Max prio = %d pid = %d\n",max_prio,retr_prio);
    return retr_prio;
}

void update_prio(int pid){
    /*
    To find:
    1. What locks does this process still hold? 
    2. For each lock --> iterate and find the max priority process and assign that to this process.
    */
    int i = 0;
    int highest_pid = 0;
    int highest_prio = 0;
    // printf("Current prio %d -> %d/%d\n",pid, proctab[pid].pprio,proctab[pid].pinh);
    for (i;i<NLOCKS;i++){
        struct mylocker *lptr;
        lptr = &lok[i];
        int checker = lptr->holders[i];
        if (checker == 1){
            int res = get_highest_prio(i);
            // printf("Potential replacement! %d\n",res);
            int inherited = proctab[res].pinh;
            int original = proctab[res].pprio;
            if (inherited >= original){
                if (inherited > highest_prio){
                    highest_prio = inherited;
                }
            }
            else{
                if (original > highest_prio){
                    highest_prio = original;
                }
            }
            // int myorig = proctab[pid].pprio;
            // int myinh = proctab[pid].pinh;
            // if (myorig > myinh){
                
            // }

        }
    }
    // if (highest_prio > proctab[pid].pprio || highest_prio > proctab[pid].pinh){
    proctab[pid].pprio = highest_prio;
        // proctab[pid].pinh = highest_prio; Should I DO THIS?
    // }
}