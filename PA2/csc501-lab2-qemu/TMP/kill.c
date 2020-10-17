/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include<lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);

	// What if I am holding a lock currently and I Am being killed? Need to release
	// Else - DEADLOCK!
	struct mylocker *lptr;
	// printf("Killing! -> %d[%d]/[%d]\n", pid, proctab[pid].pprio,proctab[pid].pinh);
	if(proctab[pid].blkid != -1){
		release_kill_chprio_logic(pid);
	}
	// check_prio_inv(pid,proctab[pid].blkid);
		// if (waiting_prio >= my_prio){
		// 	lptr = &lok[proctab[pid].blkid];
		// 	int head = lptr->lhead;
		// 	int tail = lptr->ltail;
		// 	while(tail!=head){
        //                 if (tail == -1){
        //                     break;
        //                 }
		// 				if (proctab[tail].pprio == my_prio && proctab[tail].pprio != proctab[tail].pinh){
		// 					if (proctab[tail].pinh != 0){
		// 						proctab[tail].pprio = proctab[tail].pinh;
		// 						proctab[tail].pinh = proctab[tail].origprio;
		// 					}
		// 					else{
		// 						proctab[tail].pinh = proctab[tail].pprio;
		// 					}
		// 				}
		// 		}
		// 	}
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			int i = 0;
			for(i;i<NLOCKS;i++){
				lptr = &lok[i];
				if (lptr->holders[pid] == 1){
					// This process "pid" holds this lock with descriptor "i"
					// printf("I'm here for release job\n");
					releaseall(1,i);
				}
			}
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRLWAIT:	pptr->pstate = PRFREE;
			dequeue(pid);
			break;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}

	restore(ps);
	return(OK);
}

void release_kill_chprio_logic(int pid){
	struct mylocker *lptr;
	// if(proctab[pid].blkid != -1){
		// printf("definitely here\n");
		// I'm waiting in someone's queue [What if I'm the highest prio process and I was inherited by someone else?]
		// Damn need to resolve this ASAP ROCKY!
		int waiting_ldes = proctab[pid].blkid;
		int my_prio = proctab[pid].pprio;
		lptr = &lok[waiting_ldes];
		// printf("my prio %d -> %d\n",pid,my_prio);
		// printf("And I am waiting in the lock -> %d\n", proctab[pid].blkid);
		// print_queue(proctab[pid].blkid);
		int waiting_proc = get_highest_prio(proctab[pid].blkid);
		int waiting_prio = proctab[waiting_proc].pprio;
		// printf("Highest waiting prio %d --> %d\n", waiting_proc, waiting_prio);
		int head = lptr->lhead;
		int tail = q[lptr->ltail].qprev;
		// while(tail!=head){
		// 	printf("Killer procs in wait -> %d\n", tail);
		// 	printf("Prio: %d and Inhp: %d\n",proctab[tail].pprio,proctab[tail].pinh);
		// 	if (tail == -1 || tail == 0){
        //         break;
        //     }
		// 	if (proctab[tail].pinh != 0){
		// 		printf("Baishali\n");
		// 		if (proctab[tail].pprio == my_prio){
		// 			if (proctab[tail].pprio == proctab[tail].pinh){
		// 				proctab[tail].pprio = proctab[tail].origprio;
		// 			}
		// 			else{
		// 				proctab[tail].pprio = proctab[tail].pinh;
		// 			}
		// 			// proctab[tail].pprio = proctab[tail].pinh;
		// 		}
		// 	}
		// 	tail = q[tail].qprev;
		// }
		int i = 0;
		for (i; i<NPROC;i++){
			if (lptr->holders[i] == 1 && i != pid){
				// printf("I hold this lock rn [%d] -> %d %d\n",i,proctab[i].pprio,proctab[i].pinh);
				// This proc currently holds it too -> Check it's pprio
				if (proctab[i].pinh != 0){
					// printf("Dhanraj\n");
					if (proctab[i].pprio == my_prio){
						if (proctab[i].blkid != -1){
							release_kill_chprio_logic(i);
						}
						if (proctab[i].pprio == proctab[i].pinh){
							proctab[i].pprio = proctab[i].origprio;
						}
						else{
							proctab[i].pprio = proctab[i].pinh;
						}
						int tail = q[lptr->ltail].qprev;
						if (tail == my_prio){
							check_prio_inv(tail,proctab[tail].blkid);
						}
						
					}
				}
			}
			
	}
	// }
}