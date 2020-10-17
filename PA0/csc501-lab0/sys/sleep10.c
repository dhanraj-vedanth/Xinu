/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep10(int n)
{
	STATWORD ps;   
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	} 
	if (n < 0  || clkruns==0){
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][19] += 1;
                time_matrix[currpid][19] += final_time;
        }
	         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][19] += 1;
		time_matrix[currpid][19] += final_time;
	}
	return(OK);
}
