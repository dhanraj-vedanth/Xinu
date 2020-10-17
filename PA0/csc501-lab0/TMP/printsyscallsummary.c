#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

#include <lab0.h>

int systracker = 1;

int freq_matrix[NPROC][27];
int time_matrix[NPROC][27];

char mapper[27][30] = {"sys_freemem","sys_chprio","sys_getpid","sys_getprio","sys_gettime","sys_kill","sys_receive","sys_recvclr","sys_recvtim","sys_resume","sys_scount","sys_sdelete","sys_send","sys_setdev","sys_setnok","sys_screate","sys_signal","sys_signaln","sys_sleep","sys_sleep10","sys_sleep100","sys_sleep1000","sys_sreset","sys_stacktrace","sys_suspend","sys_unsleep","sys_wait"};


void syscallsummary_start(){
	initializer();
	systracker = 1;
}


void initializer(){
	int i=0;
	int j=0;
	for (i; i<NPROC; i++){
		for (j;j<27;j++){
			freq_matrix[i][j] = 0;
			time_matrix[i][j] = 0;
		}
	}
}

void syscallsummary_stop(){
	systracker = 0;
}

void printsyscallsummary(){
	int i = 0;
	int j = 0;
	for(i=0;i<NPROC;i++){
		for(j=0;j<27;j++){
			if(freq_matrix[i][j]>0){
				printf("Process [%d]: %s\n",i,proctab[i].pname);
				printf("\tSyscall: %s, Count: %d, average execution time: %d (ms)\n",mapper[j],freq_matrix[i][j],(time_matrix[i][j]/freq_matrix[i][j]));
			}
		}
	}
}







