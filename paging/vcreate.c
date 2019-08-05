/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD ps;
	disable(ps);
	
	int *store_id = NULL;
	
	*store_id=0;
	
	int *process_id = NULL;
	
	*process_id = create(procaddr, ssize, priority, name, nargs, args);
	
	if(get_bsm(store_id)==SYSERR)
	{	
    restore(ps);
	return SYSERR;
	}
	
    if(hsize<=0 || hsize>256)
	{
		
		restore(ps);
		return SYSERR;
	}		
	
	
	int check = 0;
	
	check = bsm_map(*process_id, 4096, *store_id,hsize);
	
	
	if(check == -1)
	{
		
	   restore (ps);
	   return SYSERR;
	
    }
	
	else
	{
	proctab[*process_id].process_mapping[*store_id].priv_flag = 1;	
    		
	bsm_tab[*store_id].process_in_bs = 1;
	bsm_tab[*store_id].priv_flag=1;
 	
	proctab[*process_id].vhpno = 4096;
	proctab[*process_id].vhpnpages = hsize;
	proctab[*process_id].store = *store_id;
	
	proctab[*process_id].vmemlist = getmem(sizeof(struct mblock));
	proctab[*process_id].vmemlist ->mnext = NULL;
	proctab[*process_id].vmemlist->mlen = hsize * 4096;
    
	
	
	
	
	restore(ps);
	return *process_id;
		
		
	}
	
	
	restore(ps);
	return *process_id;
	
	
}
/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
