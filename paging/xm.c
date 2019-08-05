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
  STATWORD ps;
  
  disable(ps);
  
  if(virtpage<4096)
  {
	  restore(ps);
	  return SYSERR;
  }
  
  if(source<0 || source>7)
  {
	  restore(ps);
	  return SYSERR;
  }
  
  if(npages<1 || npages>256)
  {
	  restore(ps);
	  return SYSERR;
  }
  
 if(bsm_tab[source].bs_status == BSM_UNMAPPED){
 	restore(ps);
		return SYSERR;
	}
 
 if(proctab[currpid].process_mapping[source].bs_npages< npages)
  {
		restore(ps);
		return SYSERR;
	}
  
  int flag = bsm_map(currpid, virtpage, source, npages);
  
  if(flag== -1)
  {
	    restore(ps);
		return SYSERR;  
  }
  
  restore(ps);
  return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
  
  	STATWORD ps;
	
	disable(ps);
	
	if ((virtpage < 4096) || bsm_unmap(currpid, virtpage, 0) == -1 ) 
	{
		restore(ps);
		return SYSERR;
	}

	
	
	IN_CR3(currpid);
	restore(ps);
	return OK;
  
  
  
  return SYSERR;
}
