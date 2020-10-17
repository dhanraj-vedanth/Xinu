#ifndef _LAB1_H_
#define _LAB1_H_


#define RANDOMSCHED 1 
#define LINUXSCHED 2

int bootload;
int times;
extern int sched_type;

void setschedclass(int sched_class);
int getschedclass();
void printstuff();

// RANDOM SCHEDULER 
int prio_total();
int gen_random(int, int);
int random_sched();


// Linux Like 
int find_max_goodness();
#endif
