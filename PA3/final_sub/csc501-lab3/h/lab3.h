#ifndef LAB3
#define LAB3

int create_pgd(int);
void kill_routine();
int sc_push(int,pt_t*);
int sc_remove(int);
int sc_policy();
int pg_replacement();
void print_frames();
void cqueue_print();

void lfu_init();
int lfu_push(int ,int);
void lfu_pop();
int lfu_policy();
void pqueue_print();

#endif