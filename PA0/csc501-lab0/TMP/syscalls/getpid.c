/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][2] += 1;
		time_matrix[currpid][2] += final_time;
	}
	return(currpid);
}
