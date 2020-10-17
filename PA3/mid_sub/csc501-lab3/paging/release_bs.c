 #include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
    // kprintf("To be implemented!\n"); 
  // int bs_status;			/* MAPPED or UNMAPPED		*/
  // int bs_pid;				/* process id using this slot   */
  // int bs_vpno;				/* starting virtual page number */
  // int bs_npages;			/* number of pages in the store */
  // int bs_sem;				/* semaphore mechanism ?	*/

  bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
  bsm_tab[bs_id].bs_pid = -999;
  bsm_tab[bs_id].bs_vpno = 4096;
  bsm_tab[bs_id].bs_npages = 0;
  bsm_tab[bs_id].bs_sem = 0;
  // Github le they've called free_bsm() here why? 
  return OK;

}

