/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
	STATWORD ps;    
	register struct	sentry	*sptr;
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][16] += 1;
                time_matrix[currpid][16] += final_time;
        }
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][16] += 1;
		time_matrix[currpid][16] += final_time;
	}
	return(OK);
}
