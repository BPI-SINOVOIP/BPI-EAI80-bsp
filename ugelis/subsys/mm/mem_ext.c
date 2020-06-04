#include <stdio.h>
#include <stdlib.h>
#include "mem_ext.h"   

/*
*********************************************************************************************************
*                                    INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by RTOS to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-II.
*
* Arguments   : addr     is the starting address of the memory partition
*
*               nblks    is the number of memory blocks to create from the partition.
*
*               blksize  is the size (in bytes) of each block in the memory partition.
*
*               err      is a pointer to a variable containing an error message which will be set by
*                        this function to either:
*
*                        OS_NO_ERR            if the memory partition has been created correctly.
*                        OS_MEM_INVALID_ADDR  you are specifying an invalid address for the memory
*                                             storage of the partition.
*                        OS_MEM_INVALID_PART  no free partitions available
*                        OS_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
*                        OS_MEM_INVALID_SIZE  user specified an invalid block size
*                                             (must be greater than the size of a pointer)
* Returns    : != (OS_MEM *)0  is the partition was created
*              == (OS_MEM *)0  if the partition was not created because of invalid arguments or, no
*                              free partition is available.
*********************************************************************************************************
*/

struct os_mem  *os_mem_create (void *addr, INT32U nblks, INT32U blksize, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif
    struct os_mem    *pmem;
    INT8U     *pblk;
    void     **plink;
    INT32U     i;


#if OS_ARG_CHK_EN > 0
    if (addr == (void *)0) {                          /* Must pass a valid address for the memory part. */
        *err = OS_MEM_INVALID_ADDR;
        return ((struct os_mem *)0);
    }
    if (nblks < 2) {                                  /* Must have at least 2 blocks per partition      */
        *err = OS_MEM_INVALID_BLKS;
        return ((struct os_mem *)0);
    }
    if (blksize < sizeof(void *)) {                   /* Must contain space for at least a pointer      */
        *err = OS_MEM_INVALID_SIZE;
        return ((struct os_mem *)0);
    }
#endif
    OS_ENTER_CRITICAL();
    pmem = osmem_free_list;                             /* Get next free memory partition                */
    if (osmem_free_list != (struct os_mem *)0) {               /* See if pool of free partitions was empty      */
        osmem_free_list = (struct os_mem *)osmem_free_list->osmem_free_list;
    }
    OS_EXIT_CRITICAL();
    if (pmem == (struct os_mem *)0) {                        /* See if we have a memory partition             */
        *err = OS_MEM_INVALID_PART;
        return ((struct os_mem *)0);
    }
    plink = (void **)addr;                            /* Create linked list of free memory blocks      */
    pblk  = (INT8U *)addr + blksize;
    for (i = 0; i < (nblks - 1); i++) {
        *plink = (void *)pblk;
        plink  = (void **)pblk;
        pblk   = pblk + blksize;
    }
    *plink              = (void *)0;                  /* Last memory block points to NULL              */
    pmem->osmem_addr     = addr;                       /* Store start address of memory partition       */
    pmem->osmem_free_list = addr;                       /* Initialize pointer to pool of free blocks     */
    pmem->osmem_nfree    = nblks;                      /* Store number of free blocks in MCB            */
    pmem->osmem_nblks    = nblks;
    pmem->osmem_blk_size  = blksize;                    /* Store block size of each memory blocks        */
    *err                = OS_NO_ERR;
    return (pmem);
}

/*
*********************************************************************************************************
*                                          GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               err     is a pointer to a variable containing an error message which will be set by this
*                       function to either:
*
*                       OS_NO_ERR           if the memory partition has been created correctly.
*                       OS_MEM_NO_FREE_BLKS if there are no more free memory blocks to allocate to caller
*                       OS_MEM_INVALID_PMEM if you passed a NULL pointer for 'pmem'
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
*********************************************************************************************************
*/

void  *os_mem_get (struct os_mem *pmem, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                           /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr;
#endif
    void      *pblk;


#if OS_ARG_CHK_EN > 0
    if (pmem == (os_mem *)0) {                        /* Must point to a valid memory partition         */
        *err = OS_MEM_INVALID_PMEM;
        return ((os_mem *)0);
    }
#endif
    OS_ENTER_CRITICAL();    
    if (pmem->osmem_nfree > 0) {                       /* See if there are any free memory blocks       */
        pblk                = pmem->osmem_free_list;    /* Yes, point to next free memory block          */
        pmem->osmem_free_list = *(void **)pblk;         /*      Adjust pointer to new free list          */
        pmem->osmem_nfree--;                           /*      One less memory block in this partition  */
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;                             /*      No error                                 */
        return (pblk);                                /*      Return memory block to caller            */
    }
    OS_EXIT_CRITICAL();
    *err = OS_MEM_NO_FREE_BLKS;                       /* No,  Notify caller of empty memory partition  */
    return ((void *)0);                               /*      Return NULL pointer to caller            */
}

/*
*********************************************************************************************************
*                                         RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pblk    is a pointer to the memory block being released.
*
* Returns     : OS_NO_ERR            if the memory block was inserted into the partition
*               OS_MEM_FULL          if you are returning a memory block to an already FULL memory
*                                    partition (You freed more blocks than you allocated!)
*               OS_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               OS_MEM_INVALID_PBLK  if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/

INT8U  os_mem_put (struct os_mem  *pmem, void *pblk)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif


#if OS_ARG_CHK_EN > 0
    if (pmem == (os_mem *)0) {                   /* Must point to a valid memory partition             */
        return (OS_MEM_INVALID_PMEM);
    }
    if (pblk == (void *)0) {                     /* Must release a valid block                         */
        return (OS_MEM_INVALID_PBLK);
    }
#endif
    OS_ENTER_CRITICAL();
    if (pmem->osmem_nfree >= pmem->osmem_nblks) {  /* Make sure all blocks not already returned          */
        OS_EXIT_CRITICAL();
        return (OS_MEM_FULL);
    }
    *(void **)pblk      = pmem->osmem_free_list;   /* Insert released block into free block list         */
    pmem->osmem_free_list = pblk;
    pmem->osmem_nfree++;                          /* One more memory block in this partition            */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);                          /* Notify caller that memory block was released       */
}

/*
*********************************************************************************************************
*                                          QUERY MEMORY PARTITION
*
* Description : This function is used to determine the number of free memory blocks and the number of
*               used memory blocks from a memory partition.
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pdata   is a pointer to a structure that will contain information about the memory
*                       partition.
*
* Returns     : OS_NO_ERR            If no errors were found.
*               OS_MEM_INVALID_PMEM  if you passed a NULL pointer for 'pmem'
*               OS_MEM_INVALID_PDATA if you passed a NULL pointer for the block to release.
*********************************************************************************************************
*/

INT8U  os_mem_query (struct os_mem *pmem, struct os_mem_data *pdata)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif

#if OS_ARG_CHK_EN > 0
    if (pmem == (struct os_mem *)0) {            /* Must point to a valid memory partition             */
        return (OS_MEM_INVALID_PMEM);
    }
    if (pdata == (os_mem_data *)0) {             /* Must release a valid storage area for the data     */
        return (OS_MEM_INVALID_PDATA);
    }
#endif
    OS_ENTER_CRITICAL();
    pdata->os_addr     = pmem->osmem_addr;
    pdata->os_free_list = pmem->osmem_free_list;
    pdata->os_blk_size  = pmem->osmem_blk_size;
    pdata->os_nblks    = pmem->osmem_nblks;
    pdata->os_nfree    = pmem->osmem_nfree;
    OS_EXIT_CRITICAL();
    pdata->os_nused    = pdata->os_nblks - pdata->os_nfree;
    return (OS_NO_ERR);
}

void  os_mem_init (void)
{
#if OS_MAX_MEM_PART == 1
    osmem_free_list                = (struct os_mem *)&osmem_tbl[0]; /* Point to beginning of free list          */
    osmem_free_list->osmem_free_list = (void *)0;              /* Initialize last node                     */
    osmem_free_list->osmem_addr     = (void *)0;              /* Store start address of memory partition  */
    osmem_free_list->osmem_nfree    = 0;                      /* No free blocks                           */
    osmem_free_list->osmem_nblks    = 0;                      /* No blocks                                */
    osmem_free_list->osmem_blk_size  = 0;                      /* Zero size                                */
#endif

#if OS_MAX_MEM_PART >= 2
    struct os_mem  *pmem;
    INT16U   i;


    pmem = (struct os_mem *)&osmem_tbl[0];             /* Point to memory control block (MCB)           */
    for (i = 0; i < (OS_MAX_MEM_PART - 1); i++) {     /* Init. list of free memory partitions          */
        pmem->osmem_free_list = (void *)&osmem_tbl[i+1]; /* Chain list of free partitions                 */
        pmem->osmem_addr     = (void *)0;              /* Store start address of memory partition       */
        pmem->osmem_nfree    = 0;                      /* No free blocks                                */
        pmem->osmem_nblks    = 0;                      /* No blocks                                     */
        pmem->osmem_blk_size  = 0;                      /* Zero size                                     */
        pmem++;
    }
    pmem->osmem_free_list = (void *)0;                  /* Initialize last node                          */
    pmem->osmem_addr     = (void *)0;                  /* Store start address of memory partition       */
    pmem->osmem_nfree    = 0;                          /* No free blocks                                */
    pmem->osmem_nblks    = 0;                          /* No blocks                                     */
    pmem->osmem_blk_size  = 0;                          /* Zero size                                     */

    osmem_free_list       = (struct os_mem *)&osmem_tbl[0];     /* Point to beginning of free list               */
#endif
}
