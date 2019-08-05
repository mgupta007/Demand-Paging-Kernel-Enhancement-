/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

fr_map_t frm_tab[1024];
void EraseFrameTable(pd_t *entry, int pr, int PAGE);


/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
 
int G_PageTable[4];
 
 
 
SYSCALL init_frm()
{
   int j=0;
   STATWORD ps;
   
   do{
	   frm_tab[j].fr_dirty = 0;
	   
	   frm_tab[j].fr_pid = -1;
	   
	   frm_tab[j].fr_vpno = -1;
	   
	   frm_tab[j].fr_status = BSM_UNMAPPED;
	   
	   frm_tab[j].fr_refcnt = 0;
	   
	   frm_tab[j].fr_type = -1;
	   
	  
	   
	   j=j+1;
	   
   }while(j<1024);
   
   restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  STATWORD ps;
  disable(ps);
  
 
  *avail = 0;
  
  int flag = 0;
  
  do
  {
	  if(frm_tab[*avail].fr_status == BSM_UNMAPPED)
	  {
       restore(ps);
       return OK;
	  }  
	 
	 
 //          kprintf("Frame %d",  *avail); 
	  *avail=*avail+1;
	  
	  
  }while(*avail<1024);

  
 
 
  if(grpolicy() == 3)
  {
	 int frame_get = frame_from_SC();
	 
	
	
	*avail = frame_get;
    
	
	restore(ps);
    
	
	return OK;  
  }
  
 
  
  
  
  
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
 
 
int check_frm(int i)
{
	 int flag = 0;
	  int processID = frm_tab[i].fr_pid;
	  int virtualAdress = frm_tab[i].fr_vpno * NBPG;
	  int *store;
	  int *pageno;
	  pd_t *DirectoryEntry;
	  pt_t *TableEntry;
	  
	  flag = bsm_lookup ( processID, virtualAdress, store, pageno );
	  
	  if(flag == -1)
		  return SYSERR;
	  
	  if(flag!=-1)
	  {
	  int frNo = i + FRAME0;
	  int frA = frNo * NBPG;
	  
	  write_bs(frA, store, pageno);
	  
	  int pageTableNo = frm_tab[i].fr_vpno / 1024;
	  int pageNO = frm_tab[i].fr_vpno % 1024;
	  
	  DirectoryEntry = proctab[processID].pdbr + pageTableNo * sizeof(pd_t);
	  TableEntry = ((DirectoryEntry ->pd_base) * NBPG) + pageNO * sizeof(pt_t);
	  }
	  
	  
	  if(TableEntry->pt_pres != 0)
	  {
  

     TableEntry->pt_global = 0;
	
	TableEntry->pt_base = 0;
	
	TableEntry->pt_acc = 0;
	TableEntry->pt_avail = 0;
	
	TableEntry->pt_mbz = 0;
	
	TableEntry->pt_dirty = 0;
	
	TableEntry->pt_pwt = 0;
	
	TableEntry->pt_user = 0;
	
	TableEntry->pt_pres = 0;
	
	TableEntry->pt_pcd = 0;
	
	
	TableEntry->pt_write = 1;
  
  
	EraseFrameTable(DirectoryEntry,i,1);
	   
	return 1;
     }

     
return SYSERR;	 

}



void check_tbl(int i)
{
	int j = 0;
	int frA;
	
	do
	{
		frA= (i + FRAME0) * NBPG;
		
		pt_t *entry = frA + j * sizeof(pt_t);
         
        if(entry ->pt_pres == 1)
		{
            int value = entry -> pt_base -FRAME0;
            free_frm(value);			
	
		}	
		
		j=j+1;
		
	}while(j<1024);
	
	j = 0;
	
    do
	{
		int processID = frm_tab[i].fr_pid;
		pd_t *entryD = proctab[processID].pdbr + j * sizeof(pd_t);
		
		if(entryD -> pd_base - FRAME0 != i )
		{
			
			    j=j+1;
				continue;	
				
		}
		
		
		        entryD->pd_pres = 0;
				
				
				entryD->pd_mbz = 0;
				
				entryD->pd_user = 0;
				
				entryD->pd_base = 0;
				
				entryD->pd_pcd = 0;
				
				entryD->pd_acc = 0;
				
				entryD->pd_pwt = 0;
				
				
				
				entryD->pd_avail = 0;
				
				entryD->pd_fmb = 0;
				
				entryD->pd_write = 1;
				
				entryD->pd_global = 0;
				
		
		
		
		
		j=j+1;
		
	}while(j<1024);
	
	
}
 
 
 void check_dir(int i)
 {
	 int j=4;
	 
	 do {
			pd_t *entry = proctab[frm_tab[i].fr_pid].pdbr + j * sizeof(pd_t);
			if (entry->pd_pres != 1) {
				j=j+1;
				continue;
			}
			
			free_frm(entry->pd_base - FRAME0);
			
			j=j+1;
		}while(j<1024);
	 
 }
 
SYSCALL free_frm(int i)
{

  if(i<0)  
	  return SYSERR;
  
  if(i>1024)
  return SYSERR;
  
  int s, p;
  
  if(frm_tab[i].fr_type == FR_PAGE)
  {
	int flag = 0;
	flag = check_frm(i);
	
    if(flag == -1)
	return SYSERR;


     return OK;
  }
  
  
  if(frm_tab[i].fr_type == FR_TBL)
  {
	  
	  
	  check_tbl(i);
	  
	  EraseFrameTable(NULL,i,0);
	  
	  return OK;
	  
	  
	  
	  
  }
  
  
  
  
  if(frm_tab[i].fr_type == FR_DIR)
  {
	  

	
	check_dir(i);
		
		EraseFrameTable(NULL, i, 0);
		return OK;
	}
	
	 return OK; 
	  
	  
  }
  
  

  
 
  
  
  void creating(int flag, int processID)
  {
	 	
	frm_tab[flag].fr_refcnt = 0;
	
	frm_tab[flag].fr_dirty = 0;
	
	frm_tab[flag].fr_vpno = -1;
	
	frm_tab[flag].fr_type = FR_TBL;
	
	frm_tab[flag].fr_status = BSM_MAPPED;
	
	frm_tab[flag].fr_pid = processID;

//        kprintf("creating");
		
  }
  
  
  void creating1(int flag, int processID)
  {
	  
	  int j=0;

      int actual_frame;
	  
	  actual_frame = FRAME0 + flag; 
	  
	  do {
		
		if(j<1024)
		{
		pt_t *pageTable = (actual_frame) * NBPG + j * sizeof(pt_t);
		
		pageTable->pt_mbz = 0;
		pageTable->pt_global = 0;
		pageTable->pt_avail = 0;
		
		pageTable->pt_dirty = 0;
		
		pageTable->pt_base = 0;
		
		pageTable->pt_write = 1;
		
		pageTable->pt_pres = 0;
		
		pageTable->pt_pcd = 0;
		
		pageTable->pt_user = 0;
		pageTable->pt_pwt = 0;
		
		pageTable->pt_acc = 0;
		}
		
//	        kprintf("creating1");
                 	
		if(j>=1024)
		{
			break;
		}
		
		j=j+1;
		
	} while(1);
	  
  }
  
  
  
  int Page_Table(int processID) {

	
STATWORD (PS);
disable (PS);
	int *flag; 
         get_frm(flag);
	
	if (*flag == -1) {
        restore (PS);	
	return SYSERR;
	}
	
       //kprintf("flag %d", *flag);
	
	creating(*flag, processID);

	creating1(*flag, processID);
	
	
	restore (PS);
	return *flag;
}

  
  
  
  void P_creating(int flag, int pid)
  {
	 
	frm_tab[flag].fr_dirty = 0;
	
	frm_tab[flag].fr_pid = pid;
	frm_tab[flag].fr_refcnt = 4;
	frm_tab[flag].fr_status = BSM_MAPPED;
	frm_tab[flag].fr_type = FR_DIR;
	frm_tab[flag].fr_vpno = -1; 
	  
	  
  }
  
  void P_creating1(int flag, int pid)
  {
	  
	  int actual_frame = FRAME0 + flag;
	 proctab[pid].pdbr = actual_frame * NBPG;

	 int j=0;
	 
	do {
		pd_t *entry = proctab[pid].pdbr + (j * sizeof(pd_t));
		entry->pd_pcd = 0;
		entry->pd_acc = 0;
		entry->pd_mbz = 0;
		entry->pd_fmb = 0;
		entry->pd_global = 0;
		entry->pd_avail = 0;
		entry->pd_base = 0;
		entry->pd_pres = 0;
		entry->pd_write = 1;
		entry->pd_user = 0;
		entry->pd_pwt = 0;

		if (j >= 4) {
                       
		 j=j+1;
		 continue;
		
		}
		
		    entry->pd_pres = 1;
			entry->pd_write = 1;
			entry->pd_base = G_PageTable[j];
	
		j=j+1;
		
		
	} while(j<1024); 
  }
  
  
  
  int Page_Directory(int pid) {
	
STATWORD PS;
disable (PS);

	int *flag; 
      
        get_frm(flag);

	if (*flag != -1) {
	
    P_creating(*flag, pid);
	P_creating1(*flag,pid);


    //kprintf("%d", *flag);

       restore (PS);
	return *flag;

	
	
	}
	
        restore (PS);
	return SYSERR;
}
  
  
  
  
  
  
  void creatingglobal(int value, int i, int x)
  {
	  	G_PageTable[i] = value;

		int j1=0;
		
		do {
			pt_t *ptable = value * NBPG + j1 * sizeof(pt_t);

               		frm_tab[x].fr_refcnt++;

                 	ptable->pt_pres = 1;
			ptable->pt_write = 1;
			ptable->pt_base = j1 + i * 1024;

			frm_tab[x].fr_refcnt++;
			
			j1=j1+1;
			
		}while(j1<1024);
  }
  
  
  int initialize_Global() 
  {
	int i, j, x;
	

        STATWORD (PS);
        disable(PS);	

        i=0;
	
	do{
	
	
		x = Page_Table(0);
			

		if(x!=-1)
		{
			
		int value = 1024 + x;	
		creatingglobal(value,i,x);
		
		}
		
		
		else { 
                        restore(PS);
			return SYSERR;
		}
		
		
		
		i=i+1;
	}while(i<4);
        restore(PS);
	return OK;
}
  


  void EraseFrameTable(pd_t *entry, int pr, int PAGE) 
  {

  
    frm_tab[pr].fr_status = BSM_UNMAPPED;
	
	frm_tab[pr].fr_refcnt = 0;
	
	
	
	frm_tab[pr].fr_dirty = 0;
	
	frm_tab[pr].fr_pid = -1;
	
	frm_tab[pr].fr_vpno = -1;
	
	frm_tab[pr].fr_type = -1;
	
    int value = entry->pd_base - FRAME0 ;
	
	if (PAGE!=0 && --frm_tab[value].fr_refcnt < 1) 
	{
		free_frm(value);
		
	}

}


int IN_CR3(int processID) {
        unsigned int pointer = (proctab[processID].pdbr) / NBPG;



        unsigned int value = pointer - 1024;

        if (frm_tab[value].fr_status != BSM_MAPPED)
        {
                return SYSERR;
        }


      if(frm_tab[value].fr_pid != processID)
                 {
                         return SYSERR;
                 }

        write_cr3(proctab[processID].pdbr);
        return OK;
}






/*
 *
typedef struct {
	int ID;
        int rbit;
	struct SC_Policy *next;
}SC_Policy;

extern SC_Policy *front_SC; *rear_SC;
extern int SC_Pages;
*/

void insert_SC (int frameID)
{

int flag=0;

	SC_Policy *ptr = (SC_Policy *) getmem(sizeof(SC_Policy));
        SC_Policy *ptr1 = (SC_Policy *) getmem(sizeof(SC_Policy));

	ptr->ID = frameID;

     if(front_SC == NULL)
	 {
		 front_SC = ptr;
		 rear_SC = ptr;
		 rear_SC ->next = front_SC;
		 ptr->rbit = 0;
	 }
	 
	 else
	 {
                 ptr1 = front_SC;
                 
                 if(front_SC->ID == frameID)                  
                 front_SC ->rbit = 1;


                 if(front_SC ->ID!=frameID) 
                 ptr1= ptr1->next;

                 while(ptr1!=front_SC)
                {
                  if(frameID == ptr1->ID)
                  { 
                     ptr1->rbit = 1;  
                       
                    flag=1;
                    break;

                   }
                }
                
                if(flag==0)
               {
		 rear_SC->next = ptr;
		 rear_SC = ptr;
		 rear_SC -> next = front_SC;
                 ptr->rbit = 0;
	}
	 
	 }
	 
}






int frame_from_SC()
{
	SC_Policy *ptr;
	ptr = front_SC;
       int a;

if(front_SC->rbit == 1)
{

front_SC->rbit =0;
 SC_Policy *ptr = (SC_Policy *) getmem(sizeof(SC_Policy));
 SC_Policy *ptrv = (SC_Policy *) getmem(sizeof(SC_Policy));
 SC_Policy *ptrn = (SC_Policy *) getmem(sizeof(SC_Policy));

ptr = front_SC;

ptrv=ptr; 
ptr=ptr->next;
ptrn=ptr->next;

while(ptr!=front_SC)
{

  if(ptr->rbit == 0)
{

        a= ptr->ID;
        free_frm(a + FRAME0);
        freemem(ptr, sizeof(SC_Policy));


ptrv->next = ptrn;
break;

}

ptrv=ptrv->next;
ptr=ptr->next;
ptrn=ptrn->next;

}


}



	
      if(front_SC->rbit == 0)
{
        front_SC = front_SC->next;  	
	rear_SC ->next = front_SC;
	
        a= ptr->ID;
	free_frm(a + FRAME0);
	freemem(ptr, sizeof(SC_Policy));
}	
	return a;
	
}
  
  



  
  





