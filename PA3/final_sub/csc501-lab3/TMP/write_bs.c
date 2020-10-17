#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mark.h>
#include <bufpool.h>
#include <paging.h>

int write_bs(char *src, bsd_t bs_id, int page) {

  /* write one page of data from src
     to the backing store bs_id, page
     page.
  */
   // kprintf("Writing back frame [%d,%d] \n",bs_id,page);
   char * phy_addr = BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE + page*NBPG;
   // kprintf("Writing from BS %d Content -> %c [%x]\n",bs_id, *(src),phy_addr);
   bcopy((void*)src, phy_addr, NBPG);

   

}

