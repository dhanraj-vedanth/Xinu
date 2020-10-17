#ifndef PRINTSYS_H_
#define PRINTSYS_H_

// TASK 2

extern int etext;
extern int edata;
//extern int ebss;
void printsegaddress();

// TASK 3
//static unsigned int *esp;
void printtos();

// TASK 4
int printprocstks(int priority);

// TASK 5
unsigned long ctr1000;
extern int systracker;
extern int freq_matrix[NPROC][27];
extern int time_matrix[NPROC][27];
extern char mapper[27][30];

void syscallsummary_start();
void syscallsummary_stop();
void initializer();
void printsyscallsummary();


#endif
