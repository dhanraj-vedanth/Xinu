// /* user.c - main */

// #include <conf.h>
// #include <kernel.h>
// #include <proc.h>
// #include <stdio.h>
// #include <q.h>
// #include <lab1.h>

// /* test1.c
//  * This test program creates three processes, prA, prB, and prC, at
//  * priority 20.  The main process also has priority 20.
//  */
// // void printstuff();
// int prch(), prA, prB, prC;

// main()
// {
// 	setschedclass(2);
// 	int i;
// 	prA = create(prch,2000,20,"proc A",10,'A');
// 	prB = create(prch,2000,20,"proc B",10,'B');
// 	prC = create(prch,2000,20,"proc C",10,'C');
// 	// printstuff();
// 	// printf("Number of currently active processes --> %d\n",numproc);
// 	resume(prA);
// 	resume(prB);
// 	resume(prC);
	
// 	// printstuff();
// 	// printf("Got control #2\n");
// 	// printstuff();	
// //	prio_total();
// 	// printf("*****TRIAL 3 over****\n");
// 	// printf("Number of currently active processes --> %d\n",numproc);
// 	int k = 0;
// 	while (k<1000) {
// 		kprintf("%c",'Z');
// 		k += 1;
// 	}
// 	// printf("Got control #3\n");
	
// 	printf("\nThis is the end.\n");

// }

// prch(c)
// char c;
// {
// 	int k = 0;
// 	while(k<100000) {
// 		kprintf("%c", c);
// 		k += 1;
// 	}
// }

/*
***************************************************************************
***************************************************************************
*/

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>

#define LOOP	50

int prA, prB, prC;
int proc_a(char), proc_b(char), proc_c(char);
int proc(char);
volatile int a_cnt = 0;
volatile int b_cnt = 0;
volatile int c_cnt = 0;
volatile int s = 0;

int main() {
	int i;
	int count = 0;
	char buf[8];

	srand(1234);

	kprintf("Please Input:\n");
	while ((i = read(CONSOLE, buf, sizeof(buf))) < 1)
		;
	buf[i] = 0;
	s = atoi(buf);
	kprintf("Get %d\n", s);

	// RANDOMSCHED
	if (s < 2) {
		setschedclass(RANDOMSCHED);
		prA = create(proc_a, 2000, 10, "proc A", 1, 'A');
		prB = create(proc_b, 2000, 20, "proc B", 1, 'B');
		prC = create(proc_c, 2000, 30, "proc C", 1, 'C');
		resume(prA);
		resume(prB);
		resume(prC);
		sleep(10);
		kill(prA);
		kill(prB);
		kill(prC);

		kprintf("\nTest Result: A = %d, B = %d, C = %d\n", a_cnt, b_cnt, c_cnt);
	}
	// LINUXSCHED
	else {
		setschedclass(LINUXSCHED);
		resume(prA = create(proc, 2000, 5, "proc A", 1, 'A'));
		resume(prB = create(proc, 2000, 50, "proc B", 1, 'B'));
		resume(prC = create(proc, 2000, 90, "proc C", 1, 'C'));

		while (count++ < LOOP) {
			kprintf("M");
			// if (count == 5){
			// 	printf("Changed prio of process B\n");
			// 	chprio(47,1);
			// }
			for (i = 0; i < 1000000; i++)
				;
		}
        kprintf("\n");
	}
	return 0;
}

int proc_a(char c) {
	int i;
	kprintf("Start... %c\n", c);
	b_cnt = 0;
	c_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		a_cnt++;
	}
	return 0;
}

int proc_b(char c) {
	int i;
	kprintf("Start... %c\n", c);
	a_cnt = 0;
	c_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		b_cnt++;
	}
	return 0;
}

int proc_c(char c) {
	int i;
	kprintf("Start... %c\n", c);
	a_cnt = 0;
	b_cnt = 0;

	while (1) {
		for (i = 0; i < 10000; i++)
			;
		c_cnt++;
	}
	return 0;
}

int proc(char c) {
	int i;
	int count = 0;

	while (count++ < LOOP) {
		kprintf("%c", c);
		for (i = 0; i < 1000000; i++)
			;
	}
	return 0;
}


/*
***************************************************************************
***************************************************************************
*/

// #include <conf.h>
// #include <kernel.h>
// #include <q.h>

// /* test4.c
//  * This test program creates three processes, prA, prB, and prC, at
//  * priorities 10, 9, and 10 respectively.  The main process has priority 20.
//  *
//  * The main routine then calls chprio to change the priority of prB to be 15,
//  * while it changes its own priority to 5.
//  */

// int prch(), prA, prB, prC;

// main()
// {
// 	setschedclass(2);
// 	int i;

//         kprintf("\n\nTEST4:\n");

// 	resume(prA = create(prch,2000,10,"proc A",1,'A'));
// 	resume(prB = create(prch,2000, 9,"proc B",1,'B'));
// 	resume(prC = create(prch,2000,10,"proc C",1,'C'));

// 	printf("Prio change daw\n");
// 	chprio(prB,      15);
// 	chprio(getpid(),  5);
// 	int k = 0;
// 	while (k<5000) {
// 		kprintf("%c", 'D');
// 		for (i=0; i< 10000; i++);
// 		k++;
// 	}
// 	printf("\n");
// }

// prch(c)
// char c;
// {
// 	int i;
// 	int d = 0;
// 	while(d<5000) {
// 		kprintf("%c", c);
// 		for (i=0; i< 10000; i++);
// 		d++;
// 		printf("\n");
// 	}
	
// }
