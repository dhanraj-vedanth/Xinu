/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>
#include <lab3.h>

bs_map_t bsm_tab[BS_SIZE];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
    
//   int bs_status;			/* MAPPED or UNMAPPED		*/
//   int bs_pid;				/* process id using this slot   */
//   int bs_vpno;				/* starting virtual page number */
//   int bs_npages;			/* number of pages in the store */
//   int bs_sem;				/* semaphore mechanism ?	*/
    STATWORD ps;
    disable(ps);
    int i = 0;
    for(i; i < BS_SIZE; i++){
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        bsm_tab[i].bs_pid = -999; //No one at first
        bsm_tab[i].bs_vpno = 0;
        bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].is_pvt = 0;
        int j = 0;
        for (j; j<NPROC; j++){
            bsm_tab[i].bsm_access[j] = 0;
            bsm_tab[i].bsm_npages[j] = 0;
        }
    }
    // print_bsm();
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
    STATWORD ps;
    disable(ps);
    int i = 0;
    for(i;i<BS_SIZE; i++){
        if(bsm_tab[i].bs_status == BSM_UNMAPPED){
            *avail = i;
            restore(ps);
            return OK;
        }
    }
    *avail = -1;
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
    STATWORD ps;
    disable(ps);
    if(bsm_tab[i].bs_status == BSM_UNMAPPED){
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        bsm_tab[i].bs_pid = -999; //No one at first
        bsm_tab[i].bs_vpno = 0;
        bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].is_pvt = 0;
        restore(ps);
        return OK;
    }
    restore(ps);
    return SYSERR;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
    STATWORD ps;
    disable(ps);
    int vpno_obt = vaddr/4096;
    int i = 0;
    for (i;i<BS_SIZE;i++){
        if(bsm_tab[i].bs_status == BSM_MAPPED && bsm_tab[i].bs_pid == pid && bsm_tab[i].bs_vpno <= vpno_obt && (bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages) > vpno_obt){
            *store = i;
            *pageth = vpno_obt - bsm_tab[i].bs_vpno;
            restore(ps);
            return OK;
        }
    }
    return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
    STATWORD ps;
    disable(ps);
    if(pid < 1){
        restore(ps);
        return SYSERR;
    }
    if(source <0 || source >16){
        restore(ps);
        return SYSERR;
    }
    // What to do if already mapped?
    int i = source;
    // if (bsm_tab[i].bs_status != BSM_MAPPED){
        bsm_tab[i].bs_status = BSM_MAPPED;
        bsm_tab[i].bs_pid = pid; //No one at first
        bsm_tab[i].bs_vpno = vpno;
        bsm_tab[i].bs_npages = npages;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].is_pvt = 0;

        bsm_tab[i].bsm_access[pid] = 1;
        bsm_tab[i].bsm_npages[pid] = npages;
        

        proctab[pid].vhpno = vpno;
		proctab[pid].store = source;
        restore(ps);
        return OK;
    // }
    // else{
    //     // What do we do here?
    //     if (bsm_tab[i].is_pvt == 1){
    //         restore(ps);
    //         return SYSERR;
    //     }
    // }

}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
// WHAT IS THIS FLAG?
{
    STATWORD ps;
    disable(ps);
    if(pid <0 || pid >=50){
        restore(ps);
        return SYSERR;
    }
    int i = 0;
    for (i; i<BS_SIZE; i++){
        if (bsm_tab[i].bs_pid == pid && bsm_tab[i].bs_vpno == vpno){
            // Go ahead and unmap now
            bsm_tab[i].bs_status = BSM_UNMAPPED;
            bsm_tab[i].bs_pid = -999; 
            bsm_tab[i].bs_vpno = 4096;
            bsm_tab[i].bs_npages = 0;
            bsm_tab[i].bs_sem = 0;
            restore(ps);
            return OK;
        }
    }
    restore(ps);
    return SYSERR;
}


