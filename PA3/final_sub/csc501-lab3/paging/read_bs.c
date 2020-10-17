#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>
#include <proc.h>
#include <paging.h>

SYSCALL read_bs(char *dst, bsd_t bs_id, int page) {

  /* fetch page page from map map_id
     and write beginning at dst.
  */
  
   void * phy_addr = BACKING_STORE_BASE + bs_id*BACKING_STORE_UNIT_SIZE + page*NBPG;
   // kprintf("Reading from BS %d Content -> %c\n",bs_id,*(WORD *)phy_addr);
   bcopy(phy_addr, (void*)dst, NBPG);
}


