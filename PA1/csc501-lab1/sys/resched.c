/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int flag = 0;

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int max_pid;
	int max_goodness;
	int nproc;

	if (sched_type == LINUXSCHED){
		int pp = preempt;
		if ((bootload == 0) && preempt >0){
			// printf("First epoch da --> %d\n",currpid);
			return(OK);
		}
		else{
			bootload = 1;
			times += 1;
			if (currpid == 49 && times == 1){
				return(OK);
			}
		}
		optr = &proctab[currpid];
		int old = currpid;
		// int olprio = optr->goodness - optr->counter;
		// NEW goodness value 
		// if (optr->isnew == FALSE){
		// printstuff();
		optr->goodness = pp + optr->pprio;
		// }
		// else{
		// 	// preempt = optr->quantum;
		// 	// printstuff();
		// 	// optr->goodness = optr->pprio + pptr->quantum;
		// 	// optr->goodness = preempt + optr->pprio;
		// 	optr->isnew = FALSE;
		// }
		optr->counter = pp;
		optr->quantum = optr->counter;

		if (optr->counter <= 0 || optr->pstate!= PRCURR){
			optr->goodness = 0;
			optr->counter = 0;
		}

		max_pid = find_max_goodness();
		max_goodness = proctab[max_pid].goodness;
		nproc = max_pid;
		// printstuff();
		// printf("Currpid --> %d and elected -->%d\n",currpid,nproc);

		// if (currpid == NULLPROC){
		// 	return(OK);
		// }

		// for (p = q[rdytail].qprev; p != rdyhead; p = q[p].qprev){}
		// Re-calculating quantum values
		// printf("CURRENT PT2 [%d] --> %d, %d",currpid,optr->goodness,optr->counter);
		if ((times == 1 || optr->pstate!=PRCURR || optr->counter <= 0 || currpid == NULLPROC) && max_goodness <= 0){
			int i =0;
			for (i; i<NPROC; i ++){
				struct pentry *ptr = &proctab[i];
				if (ptr->pstate != PRFREE){
					// ptr->isnew = TRUE;
					// if (ptr->counter == 0 || ptr->counter == preempt){
					// 	ptr->quantum = ptr->pprio; 
					// }
					// else{
					ptr->quantum = ptr->pprio + (ptr->counter)/2;
					// }
					ptr->counter = ptr->quantum;
					ptr->goodness = ptr->pprio + ptr->counter;
				}
			}
			if (nonempty(rdyhead)){
				int max_pid_1 = find_max_goodness();
				max_pid = max_pid_1;
				nproc = max_pid;
				// printf("Dai found something--> %d\n",max_pid);
				max_goodness = proctab[max_pid].goodness;
			}
			else if (currpid == NULLPROC){
				return(OK);
			}
			//What to do here? 
			/*
			1. reset epoch? Yes.
			2. Need to choose the next process here as well, how would that go about now?
			3. Is it simpler to just call find_max_goodness() here now and then go on with the rest of the code?
			*/
		}
		if (optr->goodness > max_goodness && optr->pstate == PRCURR) {
			// printf(" #2 %d\n", currpid);
			// printf("Now here-->1\n");
			optr->pstate = PRCURR;
			preempt = optr->counter;
			// return currpid;
			return(OK);
		}
		
		else if (optr->goodness <= max_goodness){
			// printf(" #3 %d\n", currpid);
			preempt = optr->counter;
			nproc = max_pid;
			// printf("Now here -->2\n");
			/* REWRITE such that it matches with your cases in "resched.c"*/
			
		}
		else{
			// printf("Here??\n");
			if(((optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
		}
		}
		// printf("New proc -> %d\n",nproc);
		// if (currpid == 47){
		// 	printf("KADULA\n");
		// 	printstuff();	
		// }
		
		if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}
		nptr = &proctab[currpid = dequeue(nproc)];
		nptr->pstate = PRCURR;
		preempt = nptr->counter;
		// printf("Switching from %d to %d\n",old,nproc);
		ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
		return(OK);

	}

else{
	/* DEFAULT SCHEDULER */		
	/* no switch needed if current process priority higher than next*/
	optr = &proctab[currpid];
	if (sched_type == 0){
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	}

	/* force context switch */

	if (optr->pstate == PRCURR) {
		// printf("Part #2\n");
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* CHANGE TO SWITCH CASES LATER*/
	/* remove highest priority process at end of ready list */
	if (sched_type == 0){
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	}
/* Else if Random is chosen --> Use random_sched() logic to get the pid of the next proc*/
	else if (sched_type == RANDOMSCHED){
		// printf("ENNA DA NADAKUDHU\n");
		int next_process_no = random_sched();
		// if (next_process_no == NULLPROC){
		// 	return OK;
		// }
		nptr =&proctab[currpid =dequeue(next_process_no)];
	}
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);	
	/* The OLD process returns here when resumed. */
	// printf("Now: %d\n",currpid);
	return OK;
}
}