#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {


   if(bs_id<0 || bs_id>7)
	   return SYSERR;
   
   free_bsm(bs_id);
   IN_CR3(currpid);
  
   return OK;

}

