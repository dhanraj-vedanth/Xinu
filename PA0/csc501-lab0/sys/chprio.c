/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	
	STATWORD ps;    
	struct	pentry	*pptr;
	
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][1] += 1;
                time_matrix[currpid][1] += final_time;
        }
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][1] += 1;
		time_matrix[currpid][1] += final_time;
	}
	return(newprio);
}
