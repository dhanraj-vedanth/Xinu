/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	short	*nxtdev;
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}
	if (isbadpid(pid)){
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][13] += 1;
                time_matrix[currpid][13] += final_time;
        }
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][13] += 1;
		time_matrix[currpid][13] += final_time;
	}
	return(currpid);
	return(OK);
}
