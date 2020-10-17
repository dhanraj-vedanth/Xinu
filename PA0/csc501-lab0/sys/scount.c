/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
extern	struct	sentry	semaph[];
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}

	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][10] += 1;
                time_matrix[currpid][10] += final_time;
        }
		return(SYSERR);
	}
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][10] += 1;
		time_matrix[currpid][10] += final_time;
	}
	return(semaph[sem].semcnt);
}
