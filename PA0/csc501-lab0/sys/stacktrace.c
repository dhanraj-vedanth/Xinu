/* stacktrace.c - stacktrace */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
static unsigned long	*esp;
static unsigned long	*ebp;

#define STKDETAIL

/*------------------------------------------------------------------------
 * stacktrace - print a stack backtrace for a process
 *------------------------------------------------------------------------
 */
SYSCALL stacktrace(int pid)
{
	struct pentry	*proc = &proctab[pid];
	unsigned long	*sp, *fp;
	unsigned int start_time;
	unsigned int final_time;
	if (systracker == 1){
		start_time = ctr1000;
	}
	if (pid != 0 && isbadpid(pid)){
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][23] += 1;
                time_matrix[currpid][23] += final_time;
        }
		return SYSERR;
	}
	if (pid == currpid) {
		asm("movl %esp,esp");
		asm("movl %ebp,ebp");
		sp = esp;
		fp = ebp;
	} else {
		sp = (unsigned long *)proc->pesp;
		fp = sp + 2; 		/* where ctxsw leaves it */
	}
	kprintf("sp %X fp %X proc->pbase %X\n", sp, fp, proc->pbase);
#ifdef STKDETAIL
	while (sp < (unsigned long *)proc->pbase) {
		for (; sp < fp; sp++)
			kprintf("DATA (%08X) %08X (%d)\n", sp, *sp, *sp);
		if (*sp == MAGIC)
			break;
		kprintf("\nFP   (%08X) %08X (%d)\n", sp, *sp, *sp);
		fp = (unsigned long *) *sp++;
		if (fp <= sp) {
			kprintf("bad stack, fp (%08X) <= sp (%08X)\n", fp, sp);
			if (systracker == 1){
                		final_time = ctr1000 - start_time;
                		freq_matrix[currpid][23] += 1;
                		time_matrix[currpid][23] += final_time;
		        }
			return SYSERR;
		}
		kprintf("RET  0x%X\n", *sp);
		sp++;
	}
	kprintf("MAGIC (should be %X): %X\n", MAGIC, *sp);
	if (sp != (unsigned long *)proc->pbase) {
		kprintf("unexpected short stack\n");
		if (systracker == 1){
                final_time = ctr1000 - start_time;
                freq_matrix[currpid][23] += 1;
                time_matrix[currpid][23] += final_time;
        }
		return SYSERR;
	}
#endif
	if (systracker == 1){
		final_time = ctr1000 - start_time;
		freq_matrix[currpid][23] += 1;
		time_matrix[currpid][23] += final_time;
	}
	return(currpid);
	return OK;
}
