#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <lab3.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
  // kprintf("get_bs(%d,%d)\n",bs_id,npages);

  /* requests a new mapping of npages with ID map_id */
    // kprintf("To be implemented!\n");
    // return npages;
    // int bs_status;			/* MAPPED or UNMAPPED		*/
    // int bs_pid;				/* process id using this slot   */
    // int bs_vpno;				/* starting virtual page number */
    // int bs_npages;			/* number of pages in the store */
    // int bs_sem;				/* semaphore mechanism ?	*/
    STATWORD ps;   
    disable(ps);
    if (npages <=0 || npages >128){
      restore(ps);
      return SYSERR;
    }
    if (bs_id <0 || bs_id >= 16){
      restore(ps);
      return SYSERR;
    }
    // Should I return SYSERR if the npages requested is more than what is available for this bsm_id?
    // I think YES
    // int available_pages = 128 - bsm_tab[bs_id].bs_npages;
    // Now check if the requested npages is available
    // if (npages > available_pages){
    //   kprintf("Case 1\n");
    //   restore(ps);
    //   return SYSERR;
    // }
    if (bsm_tab[bs_id].bs_status == BSM_MAPPED){
      // kprintf("Case 2\n");
      restore(ps);
      return bsm_tab[bs_id].bs_npages;
    }
    else{
      // kprintf("Case 3\n");
      // bsm_tab[bs_id].bs_status = BSM_MAPPED;
      bsm_tab[bs_id].bs_pid = currpid;
      bsm_tab[bs_id].bs_npages = npages;
      // print_bsm();
    }
    restore(ps);
    return npages;
}
