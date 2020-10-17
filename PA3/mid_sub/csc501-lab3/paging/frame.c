/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

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
  return OK;
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
  restore(ps);
  return OK;
}

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

  int pdbr = (FRAME0 + frm_no) * 4096;
  pd_t *pd_ptr;
  pd_ptr = pdbr;
  int i = 0;
  // unsigned int pd_pres	: 1;		/* page table present?		*/
  // unsigned int pd_write : 1;		/* page is writable?		*/
  // unsigned int pd_user	: 1;		/* is use level protection?	*/
  // unsigned int pd_pwt	: 1;		/* write through cachine for pt?*/
  // unsigned int pd_pcd	: 1;		/* cache disable for this pt?	*/
  // unsigned int pd_acc	: 1;		/* page table was accessed?	*/
  // unsigned int pd_mbz	: 1;		/* must be zero			*/
  // unsigned int pd_fmb	: 1;		/* four MB pages?		*/
  // unsigned int pd_global: 1;		/* global (ignored)		*/
  // unsigned int pd_avail : 3;		/* for programmer's use		*/
  // unsigned int pd_base	: 20;		/* location of page table?	*/
  for(i;i<1024;i++){
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