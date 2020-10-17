#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

static unsigned int *ebp;
static unsigned int *esp;
void printtos(){
	asm("movl %ebp,ebp");
	asm("movl %esp,esp");
	printf("Stack Pointer Before Calling [0x%08X] --> 0x%08X\n",ebp+2, *(ebp+2));
	
        printf("Stack Pointer After Calling  [0x%08X] --> 0x%08X\n",ebp, *(ebp)); 
	printf("Stack Pointer Currently [0x%08X] --> 0x%08X\n",esp, *(esp));
	
// Populate Stack
	int var1 = 1;
	int var2 = 2;
	int var3 = 3;

	printf("Stack contents![Upto 4]:\n");
	int val = 1;
	for (val; val <5; val ++){ 
		printf("\tContent of the stack [%X] at sp+%d --> 0x%08X\n", esp+val,val, *(esp+val));
	}
	printf("\t************END OF printtos()************\n");
	
}

