/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

#include <printsegaddress.h>
#include <topofstack.h>
#include <printprocstks.h>
#include <lab0.h>


int prX;
void halt();
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

static unsigned int *esp;

prch(c)
char c;
{
	int i;
	sleep(5);	
}


int main()
{
	long zfunc = zfunction(0xaabbccaa);
	kprintf("zfunc(0xaabbccdd) = 0x%08x\n",zfunc);	
	printsegaddress();
//	unsigned int *sp;
//	asm("movl %esp,esp");
//	sp = esp;
//	printf("Stack Pointer before calling topofstack --> %X\n",esp);	
	printtos();
	printprocstks(0);
//	kprintf("\n\nHello World, Xinu lives\n\n");
	//return 0;	
	printf("\nENDIANESS CHECK: ");
        int x = 1;
        char *y = (char*)&x;
        printf("%c\n",*y+48);

	kprintf("Task 5 (printsyscallsummary)\n");
	syscallsummary_start();
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(10);
	syscallsummary_stop();
	printsyscallsummary();
}
