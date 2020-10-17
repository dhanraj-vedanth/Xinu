// /* user.c - main */

// #include <conf.h>
// #include <kernel.h>
// #include <proc.h>
// #include <stdio.h>

// /*------------------------------------------------------------------------
//  *  main  --  user main program
//  *------------------------------------------------------------------------
//  */
// int main()
// {
// 	kprintf("\n\nHello World, Xinu@QEMU lives\n\n");

//         /* The hook to shutdown QEMU for process-like execution of XINU.
//          * This API call terminates the QEMU process.
//          */
//         shutdown();
// }

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>


#define PROC1_VADDR	0x40000000
#define PROC1_VPNO      0x40000
#define PROC2_VADDR     0x80000000
#define PROC2_VPNO      0x80000
#define TEST1_BS	1

void proc1_test1(char *msg, int lck) {
	char *addr;
	int i;
	get_bs(TEST1_BS, 100);
       // kprintf("\nHi npages %d", npag);
	if (xmmap(PROC1_VPNO, TEST1_BS, 100) == SYSERR) {
		kprintf("xmmap call failed\n");
		sleep(3);
		return;
	}
	addr = (char*) PROC1_VADDR;
	//*addr = 'A';
	for (i = 0; i < 26; i++) {
		*(addr + i * NBPG) = 'A' + i;

	}
	kprintf("\nFinished writing...\n");
	sleep(6);
	// char *addr1 = (char*) BACKING_STORE_BASE + (BACKING_STORE_UNIT_SIZE *TEST1_BS);
	// for (i =0; i<26; i++){

	// }
	// kprintf("%x this is it\n",addr);
	// kprintf("IDHU first element %x %c\n",addr1, *(addr1));
	for (i = 0; i < 26; i++) {
		// kprintf("0x%08x---", bsm)
		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
	}

	xmunmap(PROC1_VPNO);
	return;
}

void proc2_test1(char *msg, int lck){
	char *addr;
	int i;
	//kprintf("\nProcess 2");
	if(xmmap(0x50000, 1, 70)==SYSERR){
		kprintf("xmmap call failed\n");
		sleep(3);
		return;
	}
        kprintf("$$$$\n");
	// for (i=0;i<10;i++){
	// 	kprintf("OTHA ENA DA IDHU BAADE\n");
	// }	
	addr = (char*)0x50000000;
	kprintf("%c ---\n",*(addr));
	for(i=0; i< 26; i++){
		// kprintf("OTHA ENA DA IDHU BAADE\n");
		kprintf("0x%08x: %c\n", addr+i*NBPG, *(addr + i *NBPG));
	}
	return;
}
void proc1_test2(char *msg, int lck) {
	int *x;

	kprintf("ready to allocate heap space\n");
	x = vgetmem(1024);
	kprintf("heap allocated at %x\n", x);
	*x = 100;
	*(x + 1) = 200;

	kprintf("heap variable: %d %d\n", *x, *(x + 1));
	vfreemem(x, 1024);
}

void proc1_test3(char *msg, int lck) {
	char *addr;
	int i;

	addr = (char*) 0x0;

	for (i = 0; i < 1024; i++) {
		*(addr + i * NBPG) = 'B';
	}

	for (i = 0; i < 1024; i++) {
		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
	}

	return;
// 	char *addr;
// 	int i;

// 	addr = (char*) 0x0;

// 	for (i = 0; i < 1024; i++) {
// 		*(addr + i * NBPG) = 'B';
// 	}
	
// 	// pt_t* ptentry = (pt_t *)((FRAME0 + 7)*NBPG);
// //	kprintf("\nBefore access %d", ptentry->pt_acc);

// 	for (i = 0; i < 1024; i++) {
// 		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
// 	}
// //	kprintf("\nAfter access %d", ptentry->pt_acc);
// 	return;
// }
}

int main() {
	// srpolicy(LFU);
	int pid1;
	int pid2;

	kprintf("\n1: shared memory\n");
	pid1 = create(proc1_test1, 2000, 20, "proc1_test1", 0, NULL);
	resume(pid1);
	sleep(10);

 	pid2 = create(proc2_test1, 2000, 20, "proc2_test1", 0, NULL);
	resume(pid2);
	sleep(10);

	
	kprintf("\n2: vgetmem/vfreemem\n");
	pid1 = vcreate(proc1_test2, 2000, 100, 20, "proc1_test2", 0, NULL);
	kprintf("pid %d has private heap\n", pid1);
	resume(pid1);
	sleep(3);

	kprintf("\n3: Frame test\n");
	pid1 = create(proc1_test3, 2000, 20, "proc1_test3", 0, NULL);
	resume(pid1);
	sleep(3); 
	//shutdown();
}



// #include <conf.h>
// #include <kernel.h>
// #include <proc.h>
// #include <stdio.h>
// #include <paging.h>
// #include <lab3.h>


// #define PROC1_VADDR	0x40000000
// #define PROC1_VPNO      0x40000
// #define PROC2_VADDR     0x80000000
// #define PROC2_VPNO      0x80000
// #define TEST1_BS	1

// void proc1_test1(char *msg, int lck){
// 	kprintf("I AM HERE BITCH\n");
// 	char *addr;
// 	int i;

// 	get_bs(TEST1_BS, 100);

// 	if (xmmap(PROC1_VPNO, TEST1_BS, 100) == SYSERR){
// 		kprintf("xmmap call failed\n");
// 		sleep(3);
// 		return;
// 	}

// 	// kprintf("Are we here?\n");
// 	addr = (char*) PROC1_VADDR;
// 	for (i = 0; i < 26; i++) {
// 		// kprintf("i ->%d\n",i);
// 		*(addr + i * NBPG) = 'A' + i;
// 	}

// 	sleep(6);

// 	for (i = 0; i < 26; i++) {
// 		// kprintf("Here???->%d\n",i);
// 		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
// 	}

// 	xmunmap(PROC1_VPNO);
// 	return;
// }
// void proc1_test1_new(char *msg, int lck){
// 	kprintf("I AM HERE BITCH\n");
// 	char *addr;
// 	int i;

// 	get_bs(TEST1_BS, 20);

// 	if (xmmap(PROC2_VPNO, TEST1_BS, 20) == SYSERR){
// 		kprintf("xmmap call failed\n");
// 		sleep(3);
// 		return;
// 	}

// 	kprintf("Are we here?\n");
// 	// addr = (char*) PROC1_VADDR;
// 	// for (i = 0; i < 26; i++) {
// 	// 	// kprintf("i ->%d\n",i);
// 	// 	*(addr + i * NBPG) = 'a' + i;
// 	// }

// 	// sleep(6);

// 	// for (i = 0; i < 26; i++) {
// 	// 	// kprintf("Here???->%d\n",i);
// 	// 	kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
// 	// }

// 	xmunmap(PROC2_VPNO);
// 	return;
// }

// void proc1_test2(char *msg, int lck) {
// 	int *x;

// 	kprintf("ready to allocate heap space\n");
// 	x = vgetmem(1024);
// 	kprintf("heap allocated at %x\n", x);
// 	*x = 100;
// 	*(x + 1) = 200;

// 	kprintf("heap variable: %d %d\n", *x, *(x + 1));
// 	vfreemem(x, 1024);
// }

// void proc1_test3(char *msg, int lck) {

// 	char *addr;
// 	int i;

// 	addr = (char*) 0x0;

// 	for (i = 0; i < 1024; i++) {
// 		*(addr + i * NBPG) = 'B';
// 	}

// 	for (i = 0; i < 1024; i++) {
// 		kprintf("0x%08x: %c\n", addr + i * NBPG, *(addr + i * NBPG));
// 	}

// 	return;
// }

// int main() {
// 	// srpolicy(LFU);
// 	int pid1;
// 	int pid2;
	
// 	kprintf("\n1: shared memory\n");
// 	pid1 = create(proc1_test1, 2000, 20, "proc1_test1", 0, NULL);
// 	pid2 = create(proc1_test1_new,2000,20,"proc1_test1-new",0,NULL);
// 	resume(pid1);
// 	resume(pid2);
// 	kprintf("Done?\n");
// 	sleep(10);

// 	// kprintf("\n2: vgetmem/vfreemem\n");
// 	// pid1 = vcreate(proc1_test2, 2000, 100, 20, "proc1_test2", 0, NULL);
// 	// kprintf("pid %d has private heap\n", pid1);
// 	// resume(pid1);
// 	// sleep(3);

// 	// kprintf("\n3: Frame test\n");
// 	// pid1 = create(proc1_test3, 2000, 20, "proc1_test3", 0, NULL);
// 	// resume(pid1);
// 	// sleep(3);
// }

