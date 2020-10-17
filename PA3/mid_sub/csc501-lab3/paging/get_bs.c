#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

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
      enable(ps);
      return SYSERR;
    }
    if (bs_id <0 || bs_id >= 16){
      enable(ps);
      return SYSERR;
    }
    if (bsm_tab[bs_id].bs_status == BSM_MAPPED){
      enable(ps);
      return bsm_tab[bs_id].bs_npages;
    }
    else{
      bsm_tab[bs_id].bs_status = BSM_MAPPED;
      bsm_tab[bs_id].bs_pid = currpid;
      bsm_tab[bs_id].bs_npages = npages;
    }
    enable(ps);
    return npages;
}


