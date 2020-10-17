/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	int oldprio = pptr->pprio;
	pptr->pprio = newprio;

	if (newprio > oldprio){
		//  There is a possibility that this now is the highest prio in the queue of the semaph
		// Need to call prio inversion on it if it is indeed waiting on a semaph wait queue
		if (proctab[pid].blkid != -1){
			check_prio_inv(pid, proctab[pid].blkid);
		}
	}
	else{
		// There is a possibility that this was the highest prio in the wait queue and now it reduced
		// Other holders must adjust accordingly 
		release_kill_chprio_logic(pid);
	}

	restore(ps);
	return(newprio);
}
