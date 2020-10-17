/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
SYSCALL resume(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][9] += 1;
		time_matrix[currpid][9] += final_time;
	}
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][9] += 1;
		time_matrix[currpid][9] += final_time;
	}
	return(prio);
}
