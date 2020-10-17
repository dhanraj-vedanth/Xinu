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
            bsm_tab[i].bsm_vpno[j] = 0;
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
    // if(bsm_tab[i].bs_status == BSM_UNMAPPED){
        bsm_tab[i].bs_status = BSM_UNMAPPED;
        // bsm_tab[i].bsm_vpno[bsm_tab[i].bs_pid]
        bsm_tab[i].bs_pid = -999; //No one at first
        bsm_tab[i].bs_vpno = 0;
        bsm_tab[i].bs_npages = 0;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].is_pvt = 0;
        
        restore(ps);
        return OK;
    // }
    // restore(ps);
    // return SYSERR;
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
    int flag = 0;
    for (i;i<BS_SIZE;i++){
        if(bsm_tab[i].is_pvt == 1 && bsm_tab[i].bs_status == BSM_MAPPED && bsm_tab[i].bs_pid == pid && 
        bsm_tab[i].bs_vpno <= vpno_obt && (bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages) > vpno_obt){
            *store = i;
            *pageth = vpno_obt - bsm_tab[i].bs_vpno;
            flag = 1;
        }
        else{
            int j = 0;
            for(j;j<NPROC;j++){
                if(j == pid && bsm_tab[i].bsm_vpno[j] <= vpno_obt 
                && (bsm_tab[i].bsm_vpno[j] + bsm_tab[i].bsm_npages[j]) > vpno_obt){
                    *store = i;
                    *pageth = vpno_obt - bsm_tab[i].bsm_vpno[j];
                    flag = 1;
                    break;
                }
            }
        }
    }
    if (flag == 1){
        restore(ps);
        return OK;
    }
    restore(ps);
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
    if (bsm_tab[i].bs_status != BSM_MAPPED){
        bsm_tab[i].bs_status = BSM_MAPPED;
        bsm_tab[i].bs_pid = pid; //No one at first
        bsm_tab[i].bs_vpno = vpno;
        bsm_tab[i].bs_npages = npages;
        bsm_tab[i].bs_sem = 0;
        bsm_tab[i].is_pvt = 0;

        bsm_tab[i].bsm_vpno[pid] = vpno;
        bsm_tab[i].bsm_npages[pid] = npages;

        proctab[pid].vhpno = vpno;
		proctab[pid].store = source;
        restore(ps);
        return OK;
    }
    else{
    //     // What do we do here?
        if (bsm_tab[i].is_pvt == 1){
            restore(ps);
            return SYSERR;
        }
        else{
            // kprintf("OHO\n");
            bsm_tab[i].bsm_vpno[pid] = vpno;
            bsm_tab[i].bsm_npages[pid] = npages;
            proctab[pid].vhpno = vpno;
            proctab[pid].store = source;
            restore(ps);
            return OK;
        }
    }

}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
// WHAT IS THIS FLAG?
{
    // kprintf("Inside bsm_unmap - %d, %d, %d\n",pid,vpno, flag);
    STATWORD ps;
    disable(ps);
    if(pid <0 || pid >=50){
        restore(ps);
        return SYSERR;
    }
    int i;
    int back_store;
    int pageth;
    unsigned long vaddr = vpno*NBPG;
    // char *addr1 = (8+NFRAMES)*NBPG;
    // kprintf("FRAME -> %c\n",*(addr1));
    i = 0;
    for (i; i<BS_SIZE; i++){
        if (flag == 1){
            // kprintf("Private heap scenes\n");
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
        else{
            // Not private - so cannot make it unmapped apidaye I think
            int j = 0;
            // kprintf("STORE -> %d\n",i);
            for (j;j<NPROC;j++){
                // kprintf("PID - %d, vpno -> %d\n",j,bsm_tab[i].bsm_vpno[j]);
                if (j == pid && bsm_tab[i].bsm_vpno[j] == vpno){
                    // kprintf("Found -> %d BS:%d [%d] to unmap\n",j,i,vpno);
                    bsm_tab[i].bsm_vpno[j] = 0;
                    bsm_tab[i].bsm_npages[j] = 0;
                    break;
                }
            }
        }
    }
            int j = 0;
            int f = 0;
            for (j;j<NPROC;j++){
                if(bsm_tab[i].bsm_npages[j] != 0){
                    f = 1;
                }
            }
            // kprintf("FLAG VALUE -> %d\n",f);
            if (f == 0){
                bsm_tab[i].bs_status = BSM_UNMAPPED;
                bsm_tab[i].bs_pid = -999;
                // print_bsm();
                restore(ps);
                return OK;
            }
            else{
                // print_bsm();
                restore(ps);
                return OK;
            }
    restore(ps);
    return SYSERR;
}


