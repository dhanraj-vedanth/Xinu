/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  // kprintf("xmmap - to be implemented!\n");
  // return SYSERR;
  // kprintf("Xmmap called by %d\n",currpid);
  STATWORD ps;
  disable(ps);
  if (source <0 || source >=16){
    restore(ps);
    return SYSERR;
  }
  if (npages <=0 || npages >128){
    restore(ps);
    return SYSERR;
  }
  if (virtpage < 4096){
    restore(ps);
    return SYSERR;
  }
  if (bsm_tab[source].is_pvt == 1){
    restore(ps);
    return SYSERR;
  }
  // if(bsm_tab[source].bs_vpno != 0 && bsm_tab[source].bs_vpno != virtpage)
	// {
	// 	kprintf("Cannot map multiple vpno to same backing store!\n");
	// 	restore(ps);
	// 	return SYSERR;
	// }
  int retr = bsm_map(currpid, virtpage, source, npages);
  if (retr == SYSERR){
    restore(ps);
    return SYSERR;
  }
  // kprintf("\n\n\n");
  // print_bsm();
  restore(ps);
  return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
  // kprintf("To be implemented!");
  // return SYSERR;
  STATWORD ps;
  disable(ps);
  // kprintf("Xmunmap called by %d[%d]\n",currpid,virtpage);
  if (virtpage < 4096){
    restore(ps);
    return SYSERR;
  }
  int back_store, pageth;

  // bsm_lookup(currpid, virtpage*4096, &bsm_id, &pageth);
  // kprintf("Obtained -> bsm_id: %d and offset: %d\n",bsm_id,pageth);
  // int bs_vpno = bsm_tab[bsm_id].bs_vpno;
  // int npages = bsm_tab[bsm_id].bs_npages;
  // // Have store and the page offset in hand.
  int i;
  for(i = 0; i < NFRAMES; i++)
        {
        if(frm_tab[i].fr_pid == currpid && frm_tab[i].fr_type == FR_PAGE)
            {
                bsm_lookup(currpid,frm_tab[i].fr_vpno *NBPG,&back_store,&pageth);
                char *addr = frm_tab[i].fr_vpno *NBPG;
                // kprintf("FR_VPNO[%d] -> %c\n",i, *(addr));  
                // kprintf("Writing back frame! %d [%d,%d] \n",i,back_store,pageth);
                write_bs( addr, back_store, pageth);
            }
        }
  // int i =0;
  // for(i; i<NFRAMES; i++){
  //   // Go throught the NFRAMES; find which has pid == currpid and is dirty
  //   if(frm_tab[i].fr_pid == currpid){
  //     kprintf("bs_vpno:%d fr_vpno:%d\n",bs_vpno, frm_tab[i].fr_vpno);
  //     // bsm_lookup(currpid, virtpage*4096, &bsm_id, &pageth);
  //     if ((frm_tab[i].fr_vpno>= bs_vpno && frm_tab[i].fr_vpno < (bs_vpno + npages))){
  //       // if (frm_tab[i].fr_dirty == 1){
  //       // Write this frame back to the BSM
  //       kprintf("Writing back to BSM [%d] - %d\n",currpid,i);
  //       write_bs((FRAME0 + i)*NBPG,bsm_id,pageth+i);
  //       // frm_tab[i].fr_dirty = 0;
  //     //  }
  //     }
  //   }
  // }

  int retr = bsm_unmap(currpid, virtpage, 0);
  // print_bsm();
  if (retr == SYSERR){
    restore(ps);
    return SYSERR;
  }
  restore(ps);
  return OK;
}
