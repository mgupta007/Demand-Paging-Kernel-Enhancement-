/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include<paging.h>
extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	
	STATWORD ps;
	disable(ps);
	
	
	if(size==0)
    {
		restore(ps);
		return SYSERR;
	}		
	
	if(block< 4096 * 4096)
	{
		restore(ps);
		return SYSERR;
	}
	
	struct mblock *ptrv = proctab[currpid].vmemlist;
	struct mblock *ptrn = ptrv->mnext;
	
	struct mblock *ptr = ptrv;
	
	
		for(ptr = ptrv; ptrn!=NULL; ptr = ptr->mnext)
	{
		ptrn = ptr->mnext;
		
       if(ptrn!=NULL && ptrn>block)
        break;
	
	}
	
	ptr->mnext = block;
	block ->mnext = ptrn;
	
	size = roundmb(size);
	block->mlen = size;
	
	restore(ps);
	
	
	
	
	
	return(OK);
}
