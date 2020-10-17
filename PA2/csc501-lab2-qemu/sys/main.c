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

/*--------------------------------Test 1--------------------------------*/
 
void reader1 (char *msg, int lck)
{
	lock (lck, READ, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock, sleep 2s\n", msg);
	sleep (2);
	kprintf ("  %s: to release lock\n", msg);
	releaseall (1, lck);
}

void test1 ()
{
	int	lck;
	int	pid1;
	int	pid2;

	kprintf("\nTest 1: readers can share the rwlock\n");
	lck  = lcreate ();
	assert (lck != SYSERR, "Test 1 failed");

	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);

	resume(pid1);
	resume(pid2);
	
	sleep (5);
	ldelete (lck);
	kprintf ("Test 1 ok\n");
}

/*----------------------------------Test 2---------------------------*/
char output2[10];
int count2;
void reader2 (char msg, int lck, int lprio)
{
        int ret;

        kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, READ, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
        kprintf ("  %c: to release lock\n", msg);
	releaseall (1, lck);
}

void writer2 (char msg, int lck, int lprio)
{
	kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, WRITE, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
        kprintf ("  %c: to release lock\n", msg);
        releaseall (1, lck);
}

void test2 ()
{
        count2 = 0;
        int     lck;
        int     rd1, rd2, rd3, rd4;
        int     wr1;

        kprintf("\nTest 2: wait on locks with priority. Expected order of"
		" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 2 failed");

	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 20);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 30);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'D', lck, 25);
	rd4 = create(reader2, 2000, 20, "reader2", 3, 'E', lck, 20);
        wr1 = create(writer2, 2000, 20, "writer2", 3, 'C', lck, 28);
	
        kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
        resume(rd1);
        // void print_queue(lck); /
        sleep (1);

        kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
        resume(wr1);
        // void print_queue(lck);
        sleep10 (1);


        kprintf("-start reader B, D, E. reader B is granted lock.\n");
        resume (rd2);
        // void print_queue(lck);
	resume (rd3);
        // void print_queue(lck);
	resume (rd4);
        // void print_queue(lck);


        sleep (15);
        kprintf("output=%s\n", output2);
        assert(mystrncmp(output2,"ABABCCDEED",10)==0,"Test 2 FAILED\n");
        kprintf ("Test 2 OK\n");
}

/*----------------------------------Test 3---------------------------*/
void reader3 (char *msg, int lck)
{
        int     ret;

        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        sleep(2);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        sleep (10);
        kprintf("  %s: to acquire lock");
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void test3 ()
{
        int     lck;
        int     rd1, rd2;
        int     wr1;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 3 failed");
        
        rd1 = create(reader3, 2000, 25, "reader3", 2, "reader A", lck);
        rd2 = create(reader3, 2000, 30, "reader3", 2, "reader B", lck);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);

        kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(wr1);
        sleep (1);

        kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
        resume(rd1);
        sleep (1);
	assert (getprio(wr1) == 25, "Test 3 failed[case1]");

        kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
        resume (rd2);
	sleep (1);
	assert (getprio(wr1) == 30, "Test 3 failed[case2]");
	
	kprintf("-kill reader B, then sleep 1s\n");
	kill (rd2);
	sleep (1);
	assert (getprio(wr1) == 25, "Test 3 failed[case3]");

	kprintf("-kill reader A, then sleep 1s\n");
	kill (rd1);
	sleep(1);
	assert(getprio(wr1) == 20, "Test 3 failed [case4]");

        sleep (8);
        kprintf ("Test 3 OK\n");
}

void reader4 (char *msg, int lck1, int lck2)
{
        int     ret;
        kprintf ("  %s: to acquire lock1\n", msg);
        lock (lck1, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock1\n", msg);
        kprintf ("  %s: to acquire lock2\n", msg);
        lock (lck2, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock2\n", msg);
        kprintf ("  %s: to release lock1 and lock2\n", msg);
        releaseall (2, lck1,lck2);
}

void test4(){
        int     lck1,lck2;
        int     rd1, rd2;
        int     wr1;

        kprintf("\nTest 4: OWN TEST CASE\n");
        lck1  = lcreate ();
        assert (lck1 != SYSERR, "Test failed");
        assert (lck2 != SYSERR, "Test failed");
        
        rd1 = create(reader3, 2000, 40, "reader3", 2, "reader A", lck1);
        rd2 = create(reader4, 2000, 30, "reader3", 2, "reader B", lck2,lck1);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck2);


        kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(wr1);
        sleep (1);
        
        kprintf("-start reader B, then sleep 1s. reader B(prio 30) Not blocked on the lock\n");
        resume (rd2);
	sleep (1);

        kprintf("-start reader A, then sleep 1s. reader A(prio 40) blocked on the lock\n");
        resume(rd1);
        sleep (1);
	// assert (getprio(wr1) == 25, "Test 3 failed[case1]");

	// assert (getprio(wr1) == 30, "Test 3 failed[case2]");
        printf("All done! :D\n");
}

void test5() {
	int lck[NLOCKS];
	int last_lck;
	int index;

	kprintf("\nTest 5: return SYSERR if no lock is available\n");

	for (index = 0; index < NLOCKS; index++) {
		lck[index] = lcreate();
		assert(lck[index] != SYSERR, "Test 5 FAILED\n");
	}

	last_lck = lcreate();
	assert(last_lck == SYSERR, "Test 5 FAILED\n");

	for (index = 0; index < NLOCKS; index++) {
		ldelete(lck[index]);
	}
	kprintf("Test 5 PASSED!\n");
}
/* *********************************************************************************** */
int testval = 1;
void reader6(char *msg, int lck) {
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	testval = testval * 10;
	sleep(3);
	testval = testval - 10;
	releaseall(1, lck);
}

void writer6(char *msg, int lck) {
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	testval = testval * 10;
	sleep(3);
	testval = testval - 5;
	releaseall(1, lck);
}

void test6() {
	int lck;
	int pid1;
	int pid2;
	testval = 1;
	kprintf("\nTest 6: write lock is mutual exclusive\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 6 FAILED\n");

	pid1 = create(writer6, 2000, 20, "writer6", 2, "writer", lck);
	pid2 = create(reader6, 2000, 20, "reader6", 2, "reader", lck);

	resume(pid1);
	sleep(2);
	resume(pid2);

	sleep(8);
	ldelete(lck);
	kill(pid1);
	kill(pid2);

	assert(testval == 40, "Test 6 FAILED\n");
	kprintf("Test 6 PASSED!\n");
}

/* *********************************************************************************** */

void reader7(char *msg, int lck) {
	int ret;

	ret = lock(lck, READ, DEFAULT_LOCK_PRIO);
	assert(ret == DELETED, "Test 7 FAILED\n");
	kprintf("Test 7 PASSED!\n");
}

void writer7(char *msg, int lck) {
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	sleep(5);
	releaseall(1, lck);

}

void test7() {
	int lck;
	int pid1;
	int pid2;

	kprintf("\nTest 7: return DELETED if lock deleted while waiting\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 7 FAILED\n");

	pid1 = create(writer7, 2000, 20, "writer7", 2, "writer", lck);
	pid2 = create(reader7, 2000, 20, "reader7", 2, "reader", lck);

	resume(pid1);
	sleep(1);

	resume(pid2);
	sleep(1);
	ldelete(lck);

	sleep(5);
	kill(pid1);
	kill(pid2);
}

/* *********************************************************************************** */

void test8() {
	int lck[5];
	int index;
	int ret;

	kprintf("\nTest 8: release multiple locks simultaneously\n");

	for (index = 0; index < 5; index++) {
		lck[index] = lcreate();
		assert(lck[index] != SYSERR, "Test 8 FAILED\n");

		ret = lock(lck[index], READ, DEFAULT_LOCK_PRIO);
		assert(ret == OK, "Test 8 FAILED\n");
	}

	ret = releaseall(2, lck[4], lck[0]);
	assert(ret == OK, "Test 8 FAILED\n");

	ret = releaseall(3, lck[1], lck[3], lck[2]);
	assert(ret == OK, "Test 8 FAILED\n");

	for (index = 0; index < 5; index++) {
		ldelete(lck[index]);
	}

	kprintf("Test 8 PASSED!\n");
}

/* *********************************************************************************** */


char output9[10];
int count9;
void reader9(char i, int lck, int lprio) {
	int ret;

	//kprintf ("  %c: to acquire lock\n", i);
	lock(lck, READ, lprio);
	output9[count9++] = i;
	//kprintf ("  %c: acquired lock, sleep 3s\n", i);
	sleep(3);
	//kprintf ("  %c: to release lock\n", i);
	output9[count9++] = i;
	releaseall(1, lck);

}

void writer9(char i, int lck, int lprio) {
	//kprintf ("  %c: to acquire lock\n", i);
	lock(lck, WRITE, lprio);
	output9[count9++] = i;
	//kprintf ("  %c: acquired lock, sleep 3s\n", i);
	sleep(3);
	//kprintf ("  %c: to release lock\n", i);
	output9[count9++] = i;
	releaseall(1, lck);

}

void test9() {
	int lck;
	int rd1, rd2, rd3, rd4;
	int wr1;

	count9 = 0;
	kprintf(
			"\nTest 9: wait on locks with priority. Expected order of "
					"lock acquisition is: reader A, reader B, reader C, writer E, reader D\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 9 FAILED\n");

	rd1 = create(reader9, 2000, 20, "reader9", 3, 'A', lck, 20);
	rd2 = create(reader9, 2000, 20, "reader9", 3, 'B', lck, 30);
	rd3 = create(reader9, 2000, 20, "reader9", 3, 'C', lck, 40);
	rd4 = create(writer9, 2000, 20, "writer9", 3, 'D', lck, 25);
	wr1 = create(writer9, 2000, 20, "writer9", 3, 'E', lck, 25);

	//kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
	resume(rd1);
	sleep(1);

	//kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
	resume(wr1);
	sleep(1);

	//kprintf("-start reader B, D, E. reader B is granted lock.\n");
	resume(rd2);
	sleep10(1);
	resume(rd3);
	sleep10(1);
	resume(rd4);

	sleep(10);
	ldelete(lck);
	kill(rd1);
	kill(rd2);
	kill(rd3);
	kill(rd4);
	kill(wr1);
	kprintf("Output is %s\n", output9);

}

/* *********************************************************************************** */

int lck1, lck2;
void procA() {
	kprintf("rdA try to acquire lock 1\n");
	int rc = lock(lck1, READ, 20);
	if (rc == OK)
		kprintf("rdA acquires lock 1, sleep 2s\n");
	else
		kprintf("unable to get lock.\n");
	sleep(2);
	kprintf("rdA releases lock 1\n");
	releaseall(1, lck1);
}

void procB() {
	kprintf("wrB try to acquire lock 2\n");
	int rc = lock(lck2, WRITE, 20);
	if (rc == OK)
		kprintf("wrB acquires lock 2, sleep 2s\n");
	else
		kprintf("unable to get lock.\n");
	sleep(2);
	kprintf("wrB releases lock 2\n");
	releaseall(1, lck2);
}

void procC() {
	kprintf("rdC try to acquire lock 1\n");
	int rc = lock(lck1, READ, 10);
	if (rc == OK)
		kprintf("rdC acquires lock 1\n");
	else
		kprintf("unable to get lock.\n");
	kprintf("rdC try to acquire lock 2\n");
	rc = lock(lck2, READ, 5);
	if (rc == OK)
		kprintf("rdC acquires lock 2, sleep 2s\n");
	else
		kprintf("unable to get lock.\n");
	sleep(2);
	kprintf("rdC releases lock 1&2\n");
	releaseall(2, lck1, lck2);
}

void procD() {
	kprintf("wrD try to acquire lock 1\n");
	int rc = lock(lck1, WRITE, 10);
	if (rc == OK)
		kprintf("wrD acquires lock 1\n");
	else
		kprintf("unable to get lock.\n");
	kprintf("wrD try to acquire lock 2\n");
	rc = lock(lck2, WRITE, 10);
	if (rc == OK)
		kprintf("wrD acquires lock 2, sleep 2s\n");
	else
		kprintf("unable to get lock.\n");
	sleep(2);
	kprintf("wrD releases lock 1&2\n");
	releaseall(2, lck1, lck2);
}

void procE() {
	kprintf("rdE try to acquire lock 1\n");
	int rc = lock(lck1, READ, 5);
	if (rc == OK)
		kprintf("rdE acquires lock 1, sleep 2s\n");
	else
		kprintf("unable to get lock.\n");
	sleep(2);
	kprintf("rdE releases lock 1\n");
	releaseall(1, lck1);
}

void procF() {
	kprintf("wrF try to acquire lock 2\n");
	int rc = lock(lck2, WRITE, 20);
	if (rc == OK)
		kprintf("wrF acquires lock 2, sleep 2s\n");
	else
		kprintf("unable to get lock.\n");
	sleep(2);
	kprintf("wrF releases lock 2\n");
	releaseall(1, lck2);
}

void test10() {
	int rdA, rdC, rdE;
	int wrB, wrD, wrF;

	kprintf(
			"\nTest 10: \nexpected output:\n rdA try to acquire lock 1\n rdA acquires lock 1, sleep 2s\n wrB try to acquire lock 2\n wrB acquires lock 2, sleep 2s\n rdC try to acquire lock 1\n rdC acquires lock 1\n rdC try to acquire lock 2\n wrD try to acquire lock 1\n rdE try to acquire lock 1\n wrF try to acquire lock 2\n rdA releases lock 1\n wrB releases lock 2\n wrF acquires lock 2, sleep 2s\n wrF releases lock 2\n rdC acquires lock 2, sleep 2s\n rdC releases lock 1&2\n wrD acquires lock 1\n wrD try to acquire lock 2\n wrD acquires lock 2, sleep 2s\n wrD releases lock 1&2\n rdE acquires lock 1, sleep 2s\n rdE releases lock 1\n\n\n real output: \n");

	kprintf("\nReal Output:\n");
	lck1 = lcreate();
	lck2 = lcreate();

	rdA = create(procA, 2000, 20, "rdA", 1, 1);
	rdC = create(procC, 2000, 20, "rdC", 1, 1);
	rdE = create(procE, 2000, 20, "rdE", 1, 1);
	wrB = create(procB, 2000, 20, "wrB", 1, 1);
	wrD = create(procD, 2000, 20, "wrD", 1, 1);
	wrF = create(procF, 2000, 20, "wrF", 1, 1);

	resume(rdA);
	resume(wrB);
	resume(rdC);
	resume(wrD);
	resume(rdE);
	resume(wrF);

	sleep(10);
	ldelete(lck1);
	ldelete(lck2);
	kill(rdA);
	kill(rdC);
	kill(rdE);
	kill(wrB);
	kill(wrD);
	kill(wrF);
}
/* *********************************************************************************** */
void reader(char *msg, int lck, int lprio) {
	int rc;
	rc = lock(lck, READ, lprio);
	if (rc == SYSERR) {
		kprintf("  %s: lock returned SYSERR\n", msg);
		return;
	}
	if (rc == DELETED) {
		kprintf("  %s: lock was DELETED\n", msg);
		return;
	}
	kprintf("  %s: acquired lock, sleep 3s\n", msg);
	sleep(3);
	releaseall(1, lck);
}

void writer(char *msg, int lck, int lprio) {
	int rc;
	rc = lock(lck, WRITE, lprio);
	if (rc == SYSERR) {
		kprintf("  %s: lock returned SYSERR\n", msg);
		return;
	}
	if (rc == DELETED) {
		kprintf("  %s: lock was DELETED\n", msg);
		return;
	}
	kprintf("  %s: acquired lock, sleep 3s\n", msg);
	sleep(3);
	releaseall(1, lck);
}
void test11() {
	int i;
	int lastlck;
	int rd1, rd2, rd3, rd4;
	int wr1;

	kprintf(
			"\nTest 11:\nexpected output:\nreader A: acquired lock, sleep 3s\nwriter C: lock was DELETED\nreader B: lock returned SYSERR\nreader D: acquired lock, sleep 3s\n");

	kprintf("\nReal Output:\n");
	// Create original lock
	lastlck = lcreate();

	// Create processes that use original lock
	rd1 = create(reader, 2000, 20, "reader", 3, "reader A", lastlck, 20);
	rd2 = create(reader, 2000, 20, "reader", 3, "reader B", lastlck, 20);
	wr1 = create(writer, 2000, 20, "writer", 3, "writer C", lastlck, 30);

	// Start reader A
	resume(rd1);
	sleep(1);

	// Start writer C. Will get returned DELETED after lock gets
	// deleted.
	resume(wr1);
	sleep(1);

	// Delete lock
	ldelete(lastlck);

	// for (i = 1; i < NLOCKS; i++) {
	// 	lastlck = lcreate();
	// 	ldelete(lastlck);
	// }

	// Create a new lock (this lock will use same slot as original)
	printf("RIGHT BEFORE BITCHESSSSSS\n");
	lastlck = lcreate();

	// Create a new process to use the new lock spot.
	rd3 = create(reader, 2000, 20, "reader", 3, "reader D", lastlck, 20);

	// Start reader B: Should get returned SYSERR since the lock it
	// uses was already deleted and a new lock is using that index
	resume(rd2);

	// Start reader D that uses new lock
	resume(rd3);
	sleep(4);
}

int main()
{
        /* These test cases are only used for test purposes.
         * The provided results do not guarantee your correctness.
         * You need to read the PA2 instruction carefully.
         */
	test1();
	test2();
	test3();
        // test4();
        // test2();

        /* The hook to shutdown QEMU for process-like execution of XINU.
         * This API call exists the QEMU process.
         */
        shutdown();
}


