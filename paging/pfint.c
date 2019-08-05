/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include<proc.h>
#include<stdio.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
 

int checkpresentpage(pd_t *entry ) 
{

    STATWORD ps;
    disable(ps);
   
	
	if (entry->pd_pres == 0) {
		
		int processID = currpid;
		
		int frame_No = Page_Table(processID);
		
		if(frame_No!=-1)
		{
		entry->pd_pres = 1;
		entry->pd_write = 1;
		entry->pd_base = frame_No + 1024;
		
		unsigned int value = (unsigned int) entry / NBPG - 1024; 
		
		frm_tab[value].fr_refcnt = frm_tab[value].fr_refcnt + 1;
	    }
		
		
		
		if (frame_No == -1) {
                        restore(ps);
			return SYSERR;
		}
		

	}
	
	restore(ps);
	return OK; 
}


int checkpresentframe()
{

        STATWORD ps;
        disable (ps);
	int frameNO;
	frameNO = get_frm();
	if (frameNO == -1) {
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	
        restore(ps);

	return frameNO;
	
}
 
 
 void setting(int flag2)
 {
	frm_tab[flag2].fr_dirty = 0;
	frm_tab[flag2].fr_pid = currpid;
	frm_tab[flag2].fr_refcnt = 1;
	frm_tab[flag2].fr_status = BSM_MAPPED;
	frm_tab[flag2].fr_type = FR_PAGE;
 }
 
 
SYSCALL pfint() {



	STATWORD ps;
	disable(ps);

//kprintf("/*//////////////////////////////////////////////////////");
	int *store, *pageth;
	
	unsigned long addre = read_cr2();

      
	
	pd_t *pd = proctab[currpid].pdbr + sizeof(pd_t) * ((addre) >> 22); //current page directory


	int flag = bsm_lookup(currpid, addre, store, pageth);
	
	
	if ( flag == -1) {
//		kprintf("%\nlu : Not Legal Address. This is not mapped\n", a);
		kill(currpid);
		restore(ps);
		return SYSERR;
	}

   
   
   int flag1 = checkpresentpage(pd);
  
   if(flag1 == -1)
	   return SYSERR;
   
	
	
	
	int flag2 = checkpresentframe();
	
	if(flag2 == -1)
		return SYSERR;
	
	
	
	//***
	setting(flag2);
	

	
	unsigned int vad = (addre / NBPG);
	vad= vad & 0x000fffff ;
	
	frm_tab[flag2].fr_vpno = vad;

	read_bs((flag2 + FRAME0) * NBPG, store, pageth);

	
	
	if (grpolicy() == SC)
		insert_SC(flag2);
	

	
	
	pt_t *Entry = ((pd->pd_base) * NBPG + sizeof(pt_t) * ((addre >> 12) & 0x000003ff));
	Entry->pt_pres = 1;
	Entry->pt_write = 1;
	Entry->pt_base = flag2 + FRAME0;
	frm_tab[(unsigned int) Entry / NBPG - FRAME0].fr_refcnt++;

	int id_process = currpid;
	
	IN_CR3(id_process);
	restore(ps);
	return OK;
}
