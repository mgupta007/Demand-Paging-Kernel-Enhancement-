#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
   //  no of pages exceed 256 + multiple virtual page nos for a single proces

   
	STATWORD ps;
	disable (ps);
	
	unsigned int store_id = bs_id;
	
	if(store_id<0 || store_id>=8)
	{
		kprintf("Invalid store");
		restore(ps);
		return SYSERR;
	}
  
  
  if(npages<1 || npages>256)
  {
	    kprintf("Invalid pages");
		restore(ps);
		return SYSERR;
	  
  }
  
  
  if(npages +  bsm_tab[store_id].total_number >256)
	  return SYSERR;
  
  
  if(bsm_tab[store_id].bs_status == BSM_MAPPED)
  {
	  if(bsm_tab[store_id].priv_flag == 1)
	  {
		kprintf("Process tried to access backing store (Private Heap)");
		restore(ps);
		return SYSERR;
		  
	  }
	  
	   if(bsm_tab[store_id].priv_flag == 0)
	  {
		
		
		
	  bsm_tab[store_id].bs_status = BSM_MAPPED;
	  bsm_tab[store_id].bs_vpno = bsm_tab[store_id].bs_npages + bsm_tab[store_id].bs_vpno;
	  bsm_tab[store_id].bs_npages = npages;  
	  bsm_tab[store_id].priv_flag=0;
	  
	  if( proctab[currpid].process_mapping[store_id].bs_status == BSM_UNMAPPED)
	  bsm_tab[store_id].process_in_bs = bsm_tab[store_id].process_in_bs + 1;
	  
	   
	
	  proctab[currpid].process_mapping[store_id].bs_status = BSM_MAPPED;
	  proctab[currpid].process_mapping[store_id].bs_pid = currpid;
	  proctab[currpid].process_mapping[store_id].bs_vpno = bsm_tab[store_id].bs_npages + bsm_tab[store_id].bs_vpno;
	  proctab[currpid].process_mapping[store_id].bs_npages = npages;
	  proctab[currpid].process_mapping[store_id].priv_flag = 0;
	  proctab[currpid].process_mapping[store_id].process_in_bs = 1;
		
		restore(ps);
		return bsm_tab[store_id].bs_npages;
		  
	  }
	  
	  
	  
  }
  
  
  if(bsm_tab[store_id].bs_status == BSM_UNMAPPED)
  {
	  restore(ps);
	  
	  
	
	  
	  bsm_tab[store_id].bs_status = BSM_MAPPED;
	  bsm_tab[store_id].bs_vpno = 0;
	  bsm_tab[store_id].bs_npages = npages;
	  bsm_tab[store_id].priv_flag = 0;
	  bsm_tab[store_id].process_in_bs = 1;
	  
	   
	
	  proctab[currpid].process_mapping[store_id].bs_status = BSM_MAPPED;
	  proctab[currpid].process_mapping[store_id].bs_pid = currpid;
	  proctab[currpid].process_mapping[store_id].bs_vpno = 0;
	  proctab[currpid].process_mapping[store_id].bs_npages = npages;
	  proctab[currpid].process_mapping[store_id].priv_flag = 0;
	  proctab[currpid].process_mapping[store_id].process_in_bs = 1;
	  
	
	
	  return npages;
  }
  
  
  
 
    return npages;

}


