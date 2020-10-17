#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lab0.h>

static unsigned long *esp;

int printprocstks(int priority){
	printf("Entered priority value: %d\n",priority);
	printf("Number of currently active processes --> %d\n",numproc);
//	printf("Current PID --> %d\n\n",currpid);
	int i = 0;
	for (i; i<= NPROC; i++){
		struct pentry *proc = &proctab[i];
//		printf("STATE --> %ld\n",proc->pstate);
//		if (proc->pstate == 3 || proc->pstate == 1){
		if (proc->pstate != 2){
			if (proc->pprio >= priority){
				printf("Process [%s]\n", proc->pname);
				printf("PID --> %d\n",i);
				if (currpid == i){
					asm("movl %esp,esp");
					printf("\tStack pointer[pesp]: 0x%08x\n", esp);
					}
				else{
					printf("\tStack pointer[pesp] --> %d\n", proc->pesp);
				}
				printf("\tStack Base[pbase] --> %d\n",proc->pbase);
				printf("\tStack Limit[plimit] --> 0x%08X\n",proc->plimit);
				printf("\tStack Size[pstklen] --> 0x%08X\n", proc->pstklen);
				printf("\tPriority -->%d\n", proc->pprio);
//				printf("\tProcess Name --> %s\n", proc->pname);
				printf("\t*****************\n");
			}
		}
	}
	printf("\t*************END OF printprocstks(int priority)**********");
return 0;
}
