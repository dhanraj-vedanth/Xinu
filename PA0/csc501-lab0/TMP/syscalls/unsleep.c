/* unsleep.c - unsleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * unsleep  --  remove  process from the sleep queue prematurely
 *------------------------------------------------------------------------
 */
SYSCALL	unsleep(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	struct	qent	*qptr;
	int	remain;
	int	next;
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}

        disable(ps);
	if (isbadpid(pid) ||
	    ( (pptr = &proctab[pid])->pstate != PRSLEEP &&
	     pptr->pstate != PRTRECV) ) {
		restore(ps);
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][25] += 1;
                time_matrix[currpid][25] += final_time;
        }
		return(SYSERR);
	}
	qptr = &q[pid];
	remain = qptr->qkey;
	if ( (next=qptr->qnext) < NPROC)
		q[next].qkey += remain;
	dequeue(pid);
	if ( (next=q[clockq].qnext) < NPROC)
		sltop = (int *) & q[next].qkey;
	else
		slnempty = FALSE;
        restore(ps);
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][25] += 1;
		time_matrix[currpid][25] += final_time;
	}
	return(OK);
}
