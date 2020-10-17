#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lab1.h>

sched_type = 0;
unsigned long	ctr1000;

// Required functions
void setschedclass(int sched_class){
    sched_type = sched_class;
}

int getschedclass(){
    return sched_type;
}

// Function to print out the queuetab or the proctab as needed. (Debugging purpose)
void printstuff(){
	printf("\n\nTEST1:\n");
	unsigned int total_sum = 0;
	// Print from proctab
	// int i = 0;
	// for (i; i<= NPROC; i++){
	// struct pentry *proc = &proctab[i];
	// if (proc->pstate != 2){
	// 	printf("Process Number: %d Process name: %s Process prio: %d\n", i, proc->pname, proc->pprio);
	// 	}
	// }
	// Print from qent
    int first;
    for (first=q[rdytail].qprev; first!= rdyhead; first=q[first].qprev){
        printf("%d | %d | %d, %d\n",first, q[first].qkey,proctab[first].goodness, proctab[first].quantum);
    }
}

// Helper functions for RANDOM SCHEDULER


// Finds the total priority value from the queuetab
int prio_total(){
    int total_prios = 0;
    int first_proc = q[rdytail].qprev;
    // total_prios += proctab[currpid].pprio;
    while(first_proc != rdyhead && first_proc < NPROC){
        // printf("Process Number [%d] --> Prio : %d\n", first_proc, q[first_proc].qkey);
        total_prios += q[first_proc].qkey; 
        first_proc = q[first_proc].qprev;
    }
    return total_prios;
}

/* 
	Generates a random number using the supplied upper and lower values
	The upper value is 1 lesser than the total prio
*/
int gen_random(int upper, int lower){
	// printf("Upper[%d] lower[%d]\n",upper,lower);
	srand(ctr1000);
    int random_num = 0;
    if (upper <= 0){}
    else{
        int random_num = (rand() % (upper - lower + 1)) + lower;
        return random_num;
    }
    return -1;
}

/*
	Main logic for the random scheduler. 
*/
int random_sched(){
    register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* Random Scheduler */
	unsigned int total_prios = 0;
	int first = q[rdytail].qprev;
	total_prios = prio_total();
	// printf("Total : %d\n", total_prios);
	int random_num = 0;
	int upper = total_prios -1;
	int lower = 0;
	random_num = gen_random(upper,lower);

// 		if (first == NULLPROC){
// 			nptr = &proctab[ (currpid = getlast(rdytail)) ];
// 			nptr->pstate = PRCURR;		/* mark it currently running	*/
// #ifdef	RTCLOCK
// 			preempt = QUANTUM;		/* reset preemption counter	*/
// #endif
	
// 			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);	
// 			return(OK);
// 		}
		// printf("RAND --> %d", random_num);
		/* Trying to handle NULLPROC */
		
		/* Looping through the ready queue to find next suitable process */
		for (first=q[rdytail].qprev; first!= rdyhead; first=q[first].qprev){
			// printf("Generated [%d] vs curr[%d]\n",random_num,q[first].qkey);
			
			if (random_num < q[first].qkey){
                return first;
			}
			else{
				
				random_num -= q[first].qkey;
				}
		}
}


/*
	Helper function for Linux like scheduler 
	Returns the PID of the process with the highest goodness to schedule it next.
*/

int find_max_goodness(){
	int proc = q[rdytail].qprev;
	int max_goodness = 0;
	int next_proc = 0;
	int i = 0;
	for(proc; proc!= rdyhead; proc= q[proc].qprev){
		if (proctab[proc].goodness > max_goodness){
			max_goodness = proctab[proc].goodness;
			next_proc = proc;
		}
	}
	// for(i; i<NPROC; i++){
	// 	struct pentry *ptr = &proctab[i];
	// 	if (ptr->pstate != PRFREE){
	// 		if (ptr->goodness > max_goodness){
	// 			max_goodness = ptr->goodness;
	// 			next_proc = i;
	// 		}
	// 	}
	// }
	return next_proc;
}


// Tried making this modular as well.
/*
int linux_like(){
	register struct	pentry	*optr;	
	register struct	pentry	*nptr;	

	optr = &proctab[currpid];
	// int unused_cpu = 
	int olprio = optr->goodness - optr->counter;
	// NEW goodness value 
	optr->goodness = preempt + olprio;
	optr->counter = preempt;

	if (preempt <= 0){
		optr->goodness = 0;
		optr->counter = 0;
	}

	max_pid = find_max_goodness();
	max_goodness = proctab[max_pid].goodness;

	// for (p = q[rdytail].qprev; p != rdyhead; p = q[p].qprev){}
	
	// Re-calculating quantum values
	if ((optr->goodness ==0 || optr->counter == 0) && max_goodness == 0){
		int i =0;
		for (i; i<NPROC; i ++){
			ptr = &proctab[i];
			if (ptr->pstate != PFREE){
				if (ptr->counter == 0 || ptr->counter == preempt){
					ptr->quantum = ptr->pprio; 
				}
				else{
					ptr->quantum = ptr->pprio + (ptr->counter)/2;
				}

				ptr->counter = ptr->quantum;
				ptr->goodness = ptr->pprio + ptr->counter;

			}
		}
		//What to do here? 
		
		// 1. reset epoch?
		// 2. Need to choose the next process here as well, how would that go about now?
		// 3. Is it simpler to just call find_max_goodness() here now and then go on with the rest of the code?
}
	if(optr->goodness > max_goodness && optr->pstate == PRCURR) {
		preempt = optr->counter
		// return currpid;
		return(OK);
	}
	else if (optr->goodness < max_goodness){
			//  REWRITE such that it matches with your cases in "resched.c"
			if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}
			nptr = &proctab[currpid = dequeue(max_pid)];
			nptr->pstate = PRCURR;
			preempt = nptr->counter;
			ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);
			return(OK)
		}
	else{
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
}
}

*/