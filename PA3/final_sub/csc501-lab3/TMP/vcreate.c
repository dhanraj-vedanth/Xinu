/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	// kprintf("To be implemented!\n");
	// return OK;
	//   int bs_status;			/* MAPPED or UNMAPPED		*/
	//   int bs_pid;				/* process id using this slot   */
	//   int bs_vpno;				/* starting virtual page number */
	//   int bs_npages;			/* number of pages in the store */
	//   int bs_sem;				/* semaphore mechanism ?	*/
	STATWORD ps;
	disable(ps);
	int pid = create(procaddr,ssize,priority,name,nargs,args);
	int bsm_id;
	int retr = get_bsm(&bsm_id);
	if (retr == SYSERR){
		restore(ps);
		return SYSERR;
	}

	int retr_2 = bsm_map(pid, 4096, bsm_id, hsize);
	if (retr_2 == SYSERR){
		restore(ps);
		return SYSERR;
	}

	bsm_tab[bsm_id].is_pvt= 1;

	// pdbr;             /* PDBR                         */
    proctab[pid].store = bsm_id;                  /* backing store for vheap      */
    proctab[pid].vhpno = 4096;                  /* starting pageno for vheap    */
    proctab[pid].vhpnpages = hsize;              /* vheap size                   */
    proctab[pid].vmemlist->mnext = NBPG * 4096;        /* vheap list              	*/

	struct mblock *new;
	new = BACKING_STORE_BASE + (bsm_id * BACKING_STORE_UNIT_SIZE);
	new->mlen = hsize * NBPG;
	new->mnext = NULL;

	restore(ps);
	return pid;

}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
