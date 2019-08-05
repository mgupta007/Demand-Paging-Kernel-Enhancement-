/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

//bs_map_t bsm_tab[8];

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
 
 

 
SYSCALL init_bsm()
{
	
	//have to initialise
	
	int j=0;
	do{
		
bsm_tab[j].bs_status = BSM_UNMAPPED;		
bsm_tab[j].bs_pid = -1;		
bsm_tab[j].bs_vpno = -1;
bsm_tab[j].bs_npages = 0;	
bsm_tab[j].bs_sem = -1;
bsm_tab[j].priv_flag = 0;
bsm_tab[j].process_in_bs = 0;

		
		
		j=j+1;
	}while(j<8);
	
	
	
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	*avail=0;
    int flag=0;
	
	do{
		
		if(bsm_tab[*avail].bs_status == BSM_UNMAPPED)
		{
			flag=1;
			break;
		}
		
		*avail++;
		
	}while(*avail<8);
	
	if(flag==0)
	return SYSERR;
	
	
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	int store_id = i;
	
	if(store_id<0 || store_id>7)
	{
		kprintf("Invalid store id");
		return SYSERR;
	}
	
	else if(bsm_tab[store_id].process_in_bs >= 1)
	{
		kprintf("cannot free entry, since process is already in backing store");
		return SYSERR;
	}
	
	
	
	
	else
	{

     bsm_tab[store_id].bs_status = BSM_UNMAPPED;		
     bsm_tab[store_id].bs_pid = -1;		
     bsm_tab[store_id].bs_vpno = -1;
     bsm_tab[store_id].bs_npages = 0;	
     bsm_tab[store_id].bs_sem = -1;
     bsm_tab[store_id].priv_flag = 0;
     bsm_tab[store_id].process_in_bs = 0;
		
	}
	
	
	
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	int j=0, flag=0;
	
	unsigned int virtual_page;
		virtual_page = vaddr/ 4096;
		virtual_page = virtual_page & 0x000fffff ; 
	
	while(j<8)
	{
		
		
		if(proctab[pid].process_mapping[j].bs_status == BSM_MAPPED)
		{
			if(virtual_page >= proctab[pid].process_mapping[j].bs_vpno  && virtual_page < (proctab[pid].process_mapping[j].bs_vpno + proctab[pid].process_mapping[j].bs_npages))
			{
				
				*pageth = virtual_page - proctab[pid].process_mapping[j].bs_vpno;
				*store = j;
				flag=1;
				break;
				
				
			}
			
			
			
		}
	  
	  j=j+1;	
		
	}
	
	
	if(flag == 0 )
	return SYSERR;

   if(flag==1)
	   return OK;
	
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	
	int store_id = source;
	
	if(store_id<0 || store_id >7)
	return SYSERR;
   
    if(npages<1 || npages>256)
    return SYSERR;		
 
 
   if(vpno<4096)
	   return SYSERR;
	
	
	if(bsm_tab[store_id].bs_status == BSM_MAPPED)
  {
	
	  if(proctab[pid].process_mapping[store_id].bs_status == BSM_UNMAPPED )
	  {
	  bsm_tab[store_id].bs_status = BSM_MAPPED;
	  bsm_tab[store_id].bs_vpno = vpno + bsm_tab[store_id].bs_npages;
	  bsm_tab[store_id].bs_npages = npages;  
	  bsm_tab[store_id].total_number += npages;  
      } 	 
	  
	  if( proctab[pid].process_mapping[store_id].bs_status != BSM_MAPPED)
	  bsm_tab[store_id].process_in_bs = bsm_tab[store_id].process_in_bs + 1;
	  
	   
	
	  proctab[pid].process_mapping[store_id].bs_status = BSM_MAPPED;
	  proctab[pid].process_mapping[store_id].bs_pid = pid;
	  proctab[pid].process_mapping[store_id].bs_vpno = vpno;
	  proctab[pid].process_mapping[store_id].bs_npages = npages;
	  //proctab[currpid].process_mapping[store_id].priv_flag = 0;
	  proctab[pid].process_mapping[store_id].process_in_bs = 1;
		
		
		
		  
	  
	  
	  
	  
  }
  
  
  if(bsm_tab[store_id].bs_status == BSM_UNMAPPED)
  {
	  
	  bsm_tab[store_id].bs_status = BSM_MAPPED;
	  bsm_tab[store_id].bs_vpno = vpno;
	  bsm_tab[store_id].bs_npages = npages;
	  //bsm_tab[store_id].priv_flag = 0;
	  bsm_tab[store_id].process_in_bs = 1;
	  bsm_tab[store_id].total_number += npages;  
	  
	   
	
	  proctab[pid].process_mapping[store_id].bs_status = BSM_MAPPED;
	  proctab[pid].process_mapping[store_id].bs_pid = pid;
	  proctab[pid].process_mapping[store_id].bs_vpno = vpno;
	  proctab[pid].process_mapping[store_id].bs_npages = npages;
	  proctab[pid].process_mapping[store_id].priv_flag = 0;
	  proctab[pid].process_mapping[store_id].process_in_bs = 1;
	  
	
	
  }
	
    
	
	
	
	
	
}



void Erase_Entry_Dictionary_Table(int vpno,int *store,int pid)
{
	int count = vpno;
		
		 
		 
		do
		{
			if(proctab[pid].process_mapping[*store].bs_npages + proctab[pid].process_mapping[*store].bs_vpno < count)
             break;
				
			unsigned int virt = count * NBPG;

			unsigned int virt1 =virt;
			virt1=virt1>>22;
			
			unsigned int virt2 =virt;
			virt2=virt2>>12;
			
			
			pd_t *Dir_Entry = proctab[pid].pdbr;
			Dir_Entry = Dir_Entry + (virt1) * sizeof(pd_t);
			
			pt_t *Table_Entry = (Dir_Entry->pd_base) * NBPG ;
             Table_Entry = Table_Entry + (sizeof(pt_t) * ((virt2) & 0x000003ff));

			int value = Table_Entry->pt_base - FRAME0 ;
			
			if (frm_tab[value].fr_status != BSM_UNMAPPED)
				free_frm(value);

			if (Dir_Entry->pd_pres == 0)
				break;
			
			count++;
		}while (proctab[pid].process_mapping[*store].bs_npages + proctab[pid].process_mapping[*store].bs_vpno > count);	
	
	
}


/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	
	int *store, *pageth;
	
	if (vpno < NBPG)
	{
		return SYSERR;
	}


	
	if (bsm_lookup(pid, (vpno * NBPG), store, pageth) == -1) {
		return SYSERR;
	}


	if (flag == 1) {
		proctab[pid].vmemlist = NULL;
	} 
	
	
	if(flag==0) {
		
		Erase_Entry_Dictionary_Table(vpno,store,pid);
	}
	
	proctab[pid].process_mapping[*store].bs_status = BSM_UNMAPPED;

	proctab[pid].process_mapping[*store].bs_pid = -1;
	
	proctab[pid].process_mapping[*store].bs_npages = 0;
	
	proctab[pid].process_mapping[*store].priv_flag = 0;
	
	proctab[pid].process_mapping[*store].bs_vpno = -1;
	
	proctab[pid].process_mapping[*store].process_in_bs = 0;
	
	
	

	bsm_tab[*store].process_in_bs = bsm_tab[*store].process_in_bs - 1 ;
	return OK;
	
	
}


