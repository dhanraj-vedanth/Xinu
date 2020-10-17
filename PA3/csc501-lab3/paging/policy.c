/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
 /*
This function will be used to set the page replacement policy to Second-Chance (SC) or Least Frequently Used (LFU). 
You can declare constant SC as 3 and LFU as 4 for this purpose.
*/

SYSCALL srpolicy(int policy)
{
  /* sanity check ! */

  // kprintf("To be implemented!\n");
  STATWORD ps;
  disable(ps);
  page_replace_policy = policy;
  enable(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
