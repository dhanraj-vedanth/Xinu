/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	STATWORD ps;   
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}
	if (n<0 || clkruns==0){
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][18] += 1;
                time_matrix[currpid][18] += final_time;
        }
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][18] += 1;
                time_matrix[currpid][18] += final_time;
        	}
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][18] += 1;
		time_matrix[currpid][18] += final_time;
	}
	return(OK);
}
