#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <lab3.h>

// void kill_routine(){
//     STATWORD ps;
//     disable(ps);
//     kprintf("Kill routine\n");
//     int i = 0;
//     int curr_store = proctab[currpid].store;
//     if (curr_store != -999){
//         release_bs(curr_store);
//     }
//     for (i;i<NFRAMES; i++){
//         // fr_status;			/* MAPPED or UNMAPPED		*/
//         // int fr_pid;				/* process id using this frame  */
//         // int fr_vpno;				/* corresponding virtual page no*/
//         // int fr_refcnt;			/* reference count		*/
//         // int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
//         // int fr_dirty;
//         if (frm_tab[i].fr_pid == currpid){
//             kprintf("freein frame %d used by %d\n",i,currpid);
//             if (frm_tab[i].fr_type == FR_PAGE){
//                 kprintf("Page remove from LL for frm no -> %d\n",i);
//                 // If the type is fr_page, it might be in the cqueue
//                 if (page_replace_policy == SC){
//                     sc_remove(i);
//                 }
//             }
//                 int vpno = frm_tab[i].fr_vpno;
//                 int bsm_id;
//                 int pageth;
//                 // int retr = bsm_lookup(currpid, vpno*4096, &bsm_id, &pageth);
//                 // if (retr != SYSERR){
//                     bsm_unmap(currpid, vpno);
//                 // }
//                 frm_tab[i].fr_pid = -999;
//                 frm_tab[i].fr_status = FRM_UNMAPPED;
//                 frm_tab[i].fr_vpno = 0;
//                 frm_tab[i].fr_dirty = 0;
//                 // frm_tab[i].fr_type = FR_PAGE;
//                 frm_tab[i].fr_refcnt = 0;
//             }
//         }
//     }
    // Remove mapping from BS? -> Done
void print_bsm(){
  kprintf("Inside print_bsm\n");
  int i = 0;
  for (i;i<16;i++){
    kprintf("-------Store:%d-------\n",i);
    kprintf("STATUS:%d, PID:%d, vpno: %d, npages:%d, pvt:%d\n",
    bsm_tab[i].bs_status,bsm_tab[i].bs_pid,bsm_tab[i].bs_vpno,
    bsm_tab[i].bs_npages,bsm_tab[i].is_pvt);
    
  }
}
void print_frames(){
    kprintf("Inside print_frames\n");
    int i = 0;
    //   int fr_status;			/* MAPPED or UNMAPPED		*/
    // int fr_pid;				/* process id using this frame  */
    // int fr_vpno;				/* corresponding virtual page no*/
    // int fr_refcnt;			/* reference count		*/
    // int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
    // int fr_dirty;
    for (i;i<1024; i++){
        kprintf("-------Frame:%d----\n",i);
        kprintf("STATUS:%d, PID: %d, vpno: %d, type:%d, ditry:%d\n",
        frm_tab[i].fr_status, frm_tab[i].fr_pid,frm_tab[i].fr_type,
        frm_tab[i].fr_dirty);
    }
    kprintf("-----------------------\n");
}

void cqueue_print(){
    kprintf("----Pages(frame_nos) in Circ Queue----\n");
    cqueue *node;
    node = chead;
    int i =0;
    int f = 0;
    while(node->next!=chead){
    // while(i<10){
        f = 1;
        kprintf("%d->",node->pg_no);
        node = node->next;
        i += 1;
    }
    if(f == 0){
        kprintf("%d->",chead->pg_no);
    }
    kprintf("\n-----------------------\n");
}

void pqueue_print(){
    kprintf("----Pages(frame_nos) in Priority Queue----\n");
    pqueue *node;
    node = phead;
    int i = 0;
    while(node->next!=NULL){
        kprintf("%d(%d)->",node->pg_no,frm_tab[node->pg_no].fr_vpno)
        ;
        node = node->next;
    }
    kprintf("\n-----------------------\n");
}