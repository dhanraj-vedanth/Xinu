/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include<paging.h>
#include<lab3.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	// kprintf("Killing process %d\n",pid);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	// kprintf("My store is %d\n", proctab[currpid].store);
	kill_routine();
    // kprintf("Out of kill routine\n");
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

void kill_routine(){
    STATWORD ps;
    disable(ps);
    // kprintf("Kill routine\n");
    int i = 0;
    int curr_store = proctab[currpid].store;
    if (curr_store != -999){
        release_bs(curr_store);
    }
    for (i;i<NFRAMES; i++){
        // fr_status;			/* MAPPED or UNMAPPED		*/
        // int fr_pid;				/* process id using this frame  */
        // int fr_vpno;				/* corresponding virtual page no*/
        // int fr_refcnt;			/* reference count		*/
        // int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
        // int fr_dirty;
        if (frm_tab[i].fr_pid == currpid){
            // kprintf("freein frame %d used by %d\n",i,currpid);
                int vpno = frm_tab[i].fr_vpno;
                int bsm_id;
                int pageth;
                // int retr = bsm_lookup(currpid, vpno*4096, &bsm_id, &pageth);
                // if (retr != SYSERR){
                    bsm_unmap(currpid, vpno);
                // }
                frm_tab[i].fr_pid = -999;
                frm_tab[i].fr_status = FRM_UNMAPPED;
                frm_tab[i].fr_vpno = 0;
                frm_tab[i].fr_dirty = 0;
                // frm_tab[i].fr_type = FR_PAGE;
                frm_tab[i].fr_refcnt = 0;
            }
        }
    }
