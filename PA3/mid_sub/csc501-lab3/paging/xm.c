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
  STATWORD ps;
  disable(ps);
  if (source <0 || source >=16){
    restore(ps);
    return SYSERR;
  }
  if (npages <=0 || npages > 128){
    restore(ps);
    return SYSERR;
  }
  if (virtpage < 4096){
    restore(ps);
    return SYSERR;
  }
  int retr = bsm_map(currpid, virtpage, source, npages)
  if (retr == SYSERR){
    restore(ps);
    return SYSERR;
  }
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
  if (virtpage < 4096){
    restore(ps);
    return SYSERR;
  }
  int retr = bsm_unmap(currpid, virtpage, 0);
  if (retr == SYSERR){
    restore(ps);
    return SYSERR;
  }
  restore(ps);
  return OK;
}
