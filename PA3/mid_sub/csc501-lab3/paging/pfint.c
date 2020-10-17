/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
  STATWORD ps;
  disable(ps);
  unsigned long virt_addr = read_cr2();
  unsigned long pdbr = proctab[currpid].pdbr;
  int vpno_from_a = virt_addr/4096; 
  int flag = -999;
  int i = 0;
  for (i;i<16;i++){
    if (bsm_tab[i].bs_pid == currpid){
      if (vpno_from_a >= bsm_tab[i].bs_vpno && vpno_from_a < (bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages)){
        flag = 1;
      }
    }
  } 
  // If flag == 1 it means that a backing store for this search and this pid exists and is valid 
  if (flag == -999){
    restore(ps);
    printf("Illegal weapon bruh %d is bring killed ASAP\n",currpid);
    kill(currpid);
    return SYSERR;
  }
  int pd_offset = (virt_addr & 0XFFC00000) >> 22;
  int pt_offest = (virt_addr & 0X003FF000) >> 12;
  int actual_offset = (virt_addr & 0X00000FFF);

  int needed_dir = pdbr + pd_offset; //ANY changes?

  pd_t *dir_pointer;

  dir_pointer = needed_dir;

  if (dir_pointer->pd_pres == 0){
    // Tells us that there is no page table present
    int new_frm;
    int retr = get_frm(&new_frm);
    if (retr == SYSERR){
      restore(ps);
      return SYSERR;
    }
    int pg_table_location = FRAME0 + new_frm;
    dir_pointer->pd_pres = 1;
    dir_pointer->pd_write = 1;
    dir_pointer->pd_user = 0;
    dir_pointer->pd_pwt = 0;
    dir_pointer->pd_pcd = 0;
    dir_pointer->pd_acc = 0;
    dir_pointer->pd_mbz = 0;
    dir_pointer->pd_global = 0;
    dir_pointer-> pd_avail = 0;
    dir_pointer->pd_base = pg_table_location;

    // Now fill 1024 entires for the pg table we just obtained
    pt_t *pt_pointer;

    pt_pointer = pg_table_location *NBPG;
    int j = 0;
    for (j;j<1024;j++{
      pt_pointer->pt_pres= 0;		/* page is present?		*/
      pt_pointer->pt_write= 0;		/* page is writable?		*/
      pt_pointer->pt_user = 0;		/* is use level protection?	*/
      pt_pointer->pt_pwt = 0;		/* write through for this page? */
      pt_pointer->pt_pcd = 0;		/* cache disable for this page? */
      pt_pointer->pt_acc = 0;		/* page was accessed?		*/
      pt_pointer->pt_dirty = 0;		/* page was written?		*/
      pt_pointer->pt_mbz = 0;		/* must be zero			*/
      pt_pointer->pt_global = 0;		/* should be zero in 586	*/
      pt_pointer->pt_avail = 0;		/* for programmer's use		*/
      pt_pointer->pt_base = 0;		/* location of page?		*/
    }
    frm_tab[new_frm].fr_status = MAPPED;
    frm_tab[new_frm].fr_pid = currpid;
    frm_tab[new_frm].fr_type = FR_TBL;
    frm_tab[new_frm].fr_vpno = virt_addr/4096;

  }
  
  // Now there is a PD and a Page table, we need to check if the page is there and if it has the needed stuff
  // But clearly it wont be there else there wouldn't have been a page fault in the first place.
  int need_pt_entry = pg_table_location*NBPG + pt_offest;
  pt_t *pt_pointer2;
  pt_pointer2 = need_pt_entry;

  if (pt_pointer2->pt_pres == 0){
    // Tells us that the needed page is indeed not there
    // Pull it from bsm, take a new frame and put it there
    frm_tab[pg_table_location-FRAME0].fr_fefcnt += 1;
    int bsm_id;
    int pg_offset;
    int retr = bsm_lookup(currpid,virt_addr, &bsm_id, &pg_offset);
    if (retr == SYSERR){
      restore(ps);
      return SYSERR;
    }
    int new_frame;
    int retr2 = get_frm(&new_frame);
    if(retr2 == SYSERR){
      restore(ps);
      return SYSERR;
      // Maybe page replacement here too? Not sure
    }
    frm_tab[new_frame].fr_status = MAPPED;
    frm_tab[new_frame].fr_pid = currpid;
    frm_tab[new_frame].fr_type = FR_PAGE;
    frm_tab[new_frame].fr_vpno = virt_addr/4096;

    new_frame_addr = (FRAME0 + new_frame) * NBPG;
    read_bs(new_frame_addr, bs_id, pg_offset);

    pt_pointer2->pt_pres = 1;
    pt_pointer2->pt_write = 1;
    pt_pointer2->pt_global = 0;
    pt_pointer2->pt_base = FRAME0 + new_frame;
  }
  restore(ps);
  // kprintf("To be implemented!\n");
  return OK;
}


