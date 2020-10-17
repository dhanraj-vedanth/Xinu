/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <lab3.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
  // int fr_status;			/* MAPPED or UNMAPPED		*/
  // int fr_pid;				/* process id using this frame  */
  // int fr_vpno;				/* corresponding virtual page no*/
  // int fr_refcnt;			/* reference count		*/
  // int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
  // int fr_dirty;

// Queue stuff?
struct pqueue;
cqueue *chead;
cqueue *start_point;
pqueue *phead;
// End of queue stuff
fr_map_t frm_tab[NFRAMES];

SYSCALL init_frm()
{
  // kprintf("To be implemented!\n");
  // return OK;
  STATWORD ps;
  disable(ps);

  int i=0;
  for(i;i<NFRAMES;i++){
    /* Iterating from 0-1023 entries */
    frm_tab[i].fr_status = FRM_UNMAPPED;
    frm_tab[i].fr_pid = -999;
    frm_tab[i].fr_vpno = 0;
    frm_tab[i].fr_refcnt = 0;
    frm_tab[i].fr_dirty = 0;
    frm_tab[i].fr_type = FR_PAGE;
    frm_tab[i].reference_cnt = 0;
  }
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  // kprintf("To be implemented!\n");
  // return OK;
  STATWORD ps;
  disable(ps);
  int i =0;
  for(i; i<NFRAMES; i++){
    if (frm_tab[i].fr_status == FRM_UNMAPPED){
      *avail = i;
      restore(ps);
      return OK;
    }
  }
  restore(ps);
  return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  // kprintf("To be implemented!\n");
  // return OK;
  STATWORD ps;
  disable(ps);
  frm_tab[i].fr_status = FRM_UNMAPPED;
  frm_tab[i].fr_pid = -999;
  frm_tab[i].fr_vpno = 0;
  frm_tab[i].fr_refcnt = 0;
  frm_tab[i].fr_dirty = 0;
  frm_tab[i].fr_type = FR_PAGE;
  // frm_tab[i].reference_cnt = 0;
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * create_pgd - Creates a new page directory
 *-------------------------------------------------------------------------*/
int create_pgd(int pid){
  STATWORD ps;
  disable(ps);
  int frm_no;
  int retr = get_frm(&frm_no);
  if (retr == SYSERR){
    restore(ps);
    return SYSERR;
  }
  // int fr_status;			/* MAPPED or UNMAPPED		*/
  // int fr_pid;				/* process id using this frame  */
  // int fr_vpno;				/* corresponding virtual page no*/
  // int fr_refcnt;			/* reference count		*/
  // int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*/
  // int fr_dirty;
  frm_tab[frm_no].fr_status = FRM_MAPPED;
  frm_tab[frm_no].fr_pid = pid;
  frm_tab[frm_no].fr_type = FR_DIR;

  unsigned long pdbr = (FRAME0 + frm_no) * 4096;
  proctab[pid].pdbr = pdbr;

  pd_t *pd_ptr;
  pd_ptr = pdbr;
  int i = 0;
  for(i;i<NFRAMES;i++){
    pd_ptr->pd_write = 1;
    if (i<4){
      // Since frames 0,1,2,3 are used as global page tables
      pd_ptr->pd_pres =1;
      pd_ptr->pd_global  = 1;
      pd_ptr->pd_base = i + FRAME0;
    }
    pd_ptr += 1;
  }
  restore(ps);
  return OK;
}

/*
Source: https://wiki.osdev.org/Paging
INVLPG is an instruction available since the i486 that invalidates a single page in the TLB. Intel notes that this instruction may be implemented differently on future processes, but that this alternate behavior must be explicitly enabled. INVLPG modifies no flags.
NASM example:
     invlpg [0]
Inline assembly for GCC (from Linux kernel source):
static inline void __native_flush_tlb_single(unsigned long addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}
*/
void invlpg(unsigned long addr){                                       
        asm volatile("invlpg (%0)" ::"r" (addr) : "memory");           
}

/*-------------------------------------------------------------------------
 * Pg_replacement() - First caller for page replaceement
 * Returns a frame number according to replacement policy 
 *-------------------------------------------------------------------------*/
int pg_replacement(){
  STATWORD ps;
  disable(ps);
  int frm_obtained;
  if(page_replace_policy == LFU){
    kprintf("LFU replacement handler invoked\n");
    frm_obtained = lfu_policy();
  }
  else{
    kprintf("Second chance replacement handler invoked\n");
    frm_obtained = sc_policy();
  }
  restore(ps);
  return frm_obtained;
}

/*-------------------------------------------------------------------------
 * sc_init(), sc_push(), sc_remove(), sc_policy() -SC ALGO related functions 
 *-------------------------------------------------------------------------*/

int sc_init(){
  STATWORD ps;
  disable(ps);
  chead = NULL;
  start_point = NULL;
  restore(ps);
  return OK;
}

int sc_push(int frame_no, pt_t *pg_ptr){
  STATWORD ps;
  disable(ps);
  // kprintf("Pushing %d in CC\n",frame_no);
  cqueue *new = (cqueue*) getmem(sizeof(cqueue));

  if (chead == NULL){
    // kprintf("chead is NULL\n");
    // This means that the queue is empty
    // Used Youtube for reference (Hope that is okay)
    // int pg_no;
    // struct cqueue *next;
    // pt_t *needed_pg; 
    chead = new;
    new->pg_no = frame_no;
    new->next = chead; //Since it's the first element it needs to circle back to itself
    new->needed_pg = pg_ptr; // needed_pg is not a pointer which has access to the pg_table structure 

    start_point = chead;
  }
  else{
    // It's not empty
    // kprintf("chead is not NULL %d\n",chead->pg_no);
    new->pg_no = frame_no;
    new->next = chead;
    new->needed_pg = pg_ptr;
    cqueue *temp;
    temp = chead;
    while(temp->next != chead){
      temp = temp->next;
      // kprintf("%d",temp->pg_no);
    }
    // At this point we are pointing at the last element in the linked list of structures!
    temp->next = new;
  }
  restore(ps);
  // cqueue_print(); 
  return OK;
}

int sc_remove(int frame_no){
  STATWORD ps;
  disable(ps);

  if (chead == NULL){
    // Means that the queue is empty, can't delete anything return syserr
    restore(ps);
    return SYSERR;
  }
  else{
    if(chead->next == chead){
      // This implies that only one element is present
      // Reset chead and the start_point
      cqueue *temp = chead;
      chead = NULL;
      start_point = NULL;
      freemem(temp, sizeof(cqueue));
    }
    else{
      // More than one element is present in the LL of structs
      // Simple idea from usual leetcode LL problems
      int flag = 0;
      int flg_for_prev = 0;
      cqueue *temp = chead;
      cqueue *prev = chead;
      while(temp->next != chead){
        if (temp->pg_no == frame_no){
          flag = 1;
          break;
        }
        if (flg_for_prev > 0){
          prev = temp;
        }
        flg_for_prev += 1;
        temp = temp->next;
      }
      if (flag == 0){
        // This frame is not even in the circular queue bro
        restore(ps);
        return SYSERR;
      }
      else{
        // Check if prev and temp are at the same point, this can happen only when the head is getting evicted
        // Make the head point to the next node
        if (temp == prev && temp == chead){
          if (start_point == temp){
            start_point = start_point->next;
          }
          cqueue *new = chead;
          while (new->next != chead){
            // Move till the last node
            new = new->next;
          }
          // New now points to the last node
          chead = chead->next;
          temp->next = NULL;
          new->next = chead;
          freemem(temp,sizeof(temp));
          }
        else{
          if (start_point == temp){
            start_point = start_point->next;
          }
          prev->next = temp->next;
          temp->next = NULL;
          freemem(temp, sizeof(temp));
        }
      }
    }
  }
  restore(ps);
  return OK;
}

int sc_policy(){
  // Go through the list of elements using the *start_point till NULL is hit
  // if the pt_acc thing is not set -> It means that this page does not deserve
  // another chance. Evict it and do something
  // When you come across a page with pt_acc set -> Reset it and move on. Give it
  // another shot. If everyone is set, you need to go through it all and then again
  // Always start form the *start_point 

  STATWORD ps;
  disable(ps);
  cqueue *node; 

  node = start_point;

  while(TRUE){
    if ((node->needed_pg)->pt_acc == 0){
      // Well this boi is NOT getting a second chance 
      // And everyone needs to eventually get here, so update the start_point here

      // How about dirty bit handling? 
      /*
      If dirty bit is set you need to find the appropriate backing store and write 
      this page deets back to the backing store. Cos the data has been modified.
      */
      pd_t *dir_pointer;
      pt_t *pt_pointer;
      int pid;
      kprintf("Evicting frame -> %d\n",node->pg_no);
      if ((node->needed_pg)->pt_dirty == 1){
        // Adei dirty bit da | Page is in hand 
        // You have frame no, and pointer to this page table entry in hand
        // Marking the page as Not present in it's page table entry
        (node->needed_pg)->pt_pres = 0;
        int frm_no;
        frm_no = node->pg_no;
        int vpno;
        vpno = frm_tab[frm_no].fr_vpno;
        pid = frm_tab[frm_no].fr_pid;
        unsigned int vaddr = vpno * 4096;
        int pd_offset = (vaddr & 0XFFC00000) >> 22;
        int pt_offest = (vaddr & 0X003FF000) >> 12;
        
        dir_pointer = proctab[pid].pdbr + pd_offset * sizeof(pd_t);
        pt_pointer = dir_pointer->pd_base *NBPG + (pt_offest *sizeof(pt_t));
        // If the fame being evicted belongs to the currpid?
        // ******HANDLE IT*********//
        if (frm_tab[frm_no].fr_pid == currpid){
          invlpg(vaddr);
        }
        int bsm_id;
        int pageth;
        int retr = bsm_lookup(currpid, vaddr, &bsm_id,&pageth);
        if (retr == SYSERR){
          restore(ps);
          return SYSERR;
        }
        // Write it back to the backing store 
        write_bs((FRAME0 + frm_no)*NBPG, bsm_id, pageth);
        (node->needed_pg)->pt_dirty = 0;
        pt_pointer->pt_pres = 0;
        // Need to subtract rf_cnt from pt_table
        // frm_tab[frm_no].fr_refcnt -= 1;
        frm_tab[dir_pointer->pd_base - FRAME0].reference_cnt -=1;

        if (frm_tab[dir_pointer->pd_base - FRAME0].reference_cnt == 0){
          // set reference count in pg dir to 0 too
          free_frm(dir_pointer->pd_base - FRAME0);
          dir_pointer->pd_pres = 0;
        }
        // start_point->needed_pg = ptable_entry; //Need to take care of that 
				start_point = node->next;
        // A frame number has been elected remove that frame from the SC LL
        sc_remove(frm_no); //Think if this is needed 
        restore(ps);
        return frm_no;
      }
      else{
        // Dirty bit not set, directly evict I guess? 
        int frm_no;
        frm_no = node->pg_no;
        int vpno;
        vpno = frm_tab[frm_no].fr_vpno;
        pid = frm_tab[frm_no].fr_pid;
        unsigned int vaddr = vpno * 4096;
        int pd_offset = (vaddr & 0XFFC00000) >> 22;
        int pt_offest = (vaddr & 0X003FF000) >> 12;
        
        dir_pointer = proctab[pid].pdbr + pd_offset * sizeof(pd_t);
        pt_pointer = dir_pointer->pd_base *NBPG + (pt_offest *sizeof(pt_t));
        frm_tab[dir_pointer->pd_base - FRAME0].reference_cnt -=1;
        pt_pointer->pt_pres = 0;

        if (frm_tab[dir_pointer->pd_base - FRAME0].reference_cnt == 0){
          // set reference count in pg dir to 0 too
          free_frm(dir_pointer->pd_base - FRAME0);
          dir_pointer->pd_pres = 0;
        }
        // start_point->needed_pg = ptable_entry; //Need to take care of that 
				start_point = node->next;
        // A frame number has been elected remove that frame from the SC LL
        sc_remove(frm_no);
        restore(ps);
        return frm_no;
      }
      
    }
    else{
      // This boi gets another shot
      (node->needed_pg)->pt_acc = 0;
      node = node->next;
    }
  }
}

/*-------------------------------------------------------------------------
 * SC POLICY DEFINITIONS END HERE
 *-------------------------------------------------------------------------*/


 /*-------------------------------------------------------------------------
 * LFU POLICY DEFINITIONS START HERE
 *-------------------------------------------------------------------------*/

 void lfu_init(){
   STATWORD ps;
   disable(ps);
   phead = NULL;
   restore(ps);
   return OK;
 }

 int lfu_push(int frame_number,int fr_refcnt){
   STATWORD ps;
   disable(ps);
  //  kprintf("Pushing %d in Prio queue\n",frame_number);
   pqueue *new = (pqueue*) getmem(sizeof(pqueue));
   new->pg_no = frame_number;
   new->count = fr_refcnt;
   if (phead == NULL){
    // kprintf("phead is NULL\n");
      // This means that the queue is empty
      // Used Youtube for reference (Hope that is okay)
      // int pg_no;
      // struct pqueue *next;
    phead = new;
    new->next = NULL; //Since it's the first element it needs to circle back to itself
  }
  else{
    pqueue *node;
    node = phead;
    int flag_for_prev = 0;
    if(new->count < node->count){
      new->next = node;
      phead = new;
      node->next = NULL;
    }
    else { 

        // Traverse the list and find a 
        // position to insert new node 
        while (node->next != NULL &&
               (node->next)->count < new->count) { 
            node = node->next; 
        } 
  
        // Either at the ends of the list 
        // or at required position 
        if ((node->next)->count != new->count){
          new->next = node->next; 
          node->next = new; 
        }
        else{
          int existiting_fr = node->next->pg_no;
          if(frm_tab[frame_number].fr_vpno > frm_tab[existiting_fr].fr_vpno){
            new->next = node->next; 
            node->next = new; 
          }
          else{
            pqueue *temp;
            temp = node->next;
            new->next = temp->next;
            temp->next = new;
          }
        }
    }
  }
  restore(ps);
  return OK;
 } 

void lfu_pop(){
  STATWORD ps;
  disable(ps);
  pqueue *node;
  node = phead;
  phead = phead->next;
  node->next = NULL;
  freemem(node,sizeof(node));
  restore(ps);
  return OK;
}

int lfu_policy(){
  STATWORD ps;
  disable(ps);
  int retr_frame;
  retr_frame = phead->pg_no;
  int vpno;
  int pid;
  pd_t *dir_pointer;
  pt_t *pt_pointer;
  vpno = frm_tab[retr_frame].fr_vpno;
  pid = frm_tab[retr_frame].fr_pid;
  unsigned int vaddr = vpno * 4096;
  int bsm_id;
  int pageth;
  int retr = bsm_lookup(currpid, vaddr, &bsm_id,&pageth);
  if (retr == SYSERR){
    restore(ps);
    return SYSERR;
  }
  int pd_offset = (vaddr & 0XFFC00000) >> 22;
  int pt_offest = (vaddr & 0X003FF000) >> 12;
  
  dir_pointer = proctab[pid].pdbr + pd_offset * sizeof(pd_t);
  pt_pointer = dir_pointer->pd_base *NBPG + (pt_offest *sizeof(pt_t));
  if (pt_pointer->pt_dirty == 1){
    write_bs((FRAME0 + retr_frame)*NBPG, bsm_id,pageth);
  }
  frm_tab[dir_pointer->pd_base - FRAME0].reference_cnt -=1;
  pt_pointer->pt_pres = 0;

  if (frm_tab[dir_pointer->pd_base - FRAME0].reference_cnt == 0){
    // set reference count in pg dir to 0 too
    free_frm(dir_pointer->pd_base - FRAME0);
    dir_pointer->pd_pres = 0;
  }
  if (frm_tab[retr_frame].fr_pid == currpid){
    invlpg(vaddr);
  }
  lfu_pop();
  kprintf("Evicting frame -> %d\n",retr_frame);
  restore(ps);
  return retr_frame;
} 





 
