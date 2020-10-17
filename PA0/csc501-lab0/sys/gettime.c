/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
#include <lab0.h>
extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
    unsigned int start_time;
    unsigned int final_time;
    if (systracker == 1){
        start_time = ctr1000;
	}
    if (systracker == 1){
        final_time = ctr1000 - start_time;
        freq_matrix[currpid][4] += 1;
        time_matrix[currpid][4] += final_time;
	}
    return OK;
}
