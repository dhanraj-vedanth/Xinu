#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }

// void reader4(char*,int,int);
int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}


void semaphore(char *msg,int sema){
    kprintf("%s is trying to acquire the semaphore\n",msg);
    int x = wait(sema);
    sleep(2);
    kprintf("%s has acquired the lock ! \n",msg);
	sleep(10);
    signal(sema);
	kprintf("%s released the semaphore\n",msg);
}

void reader_test(char *msg, int lck)
{
        int ret;
		printf("%s I am a Dummy process\n",msg);
		sleep(10);
		printf("%s I will wait for a bit now\n",msg);
}

void writer_test(char *msg, int lck)
{
        kprintf ("%s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("%s: acquired lock, sleep 10s\n", msg);
        sleep (5);
        kprintf("%s: to acquire lock");
        kprintf ("%s: to release lock\n", msg);
		sleep(5);
        releaseall (1, lck);
}


int main(){

        kprintf("\n-----------------------------------------\n");
        kprintf("Output for priority inversion problem with Semaphores\n\n");
        int sem = screate(1);
        int sem_low = create(semaphore,2000,20,"A",1,"low_prio proc",sem_low);
        int sem_med = create(semaphore,2000,25,"B",1,"med_prio proc",sem_med);
        int sem_high = create(semaphore,2000,40,"C",1,"high_prio proc",sem_high);

        resume(sem_low);
		sleep(1);
        resume(sem_med);
        sleep(2);
        resume(sem_high);
        sleep(10);

        kprintf("\n-----------------------------------------\n");
        kprintf("Priority inversion test case with LOCKS(priority_inheritance)\n\n");

		int lck;
        int lock_low,lock_med,lock_high;

        lck = lcreate ();
        lock_low = create(writer_test,2000,20,"A",2,"low_prio proc",lck);
        lock_med = create(reader_test,2000,25,"B",2,"med_prio proc",lck);
        lock_high = create(writer_test,2000,40,"C",2,"high_prio proc",lck);
        
        resume(lock_low);
		sleep(2);
        resume(lock_med);
        sleep(2);
        resume(lock_high);

        sleep(15);
        shutdown();
        return OK;

}