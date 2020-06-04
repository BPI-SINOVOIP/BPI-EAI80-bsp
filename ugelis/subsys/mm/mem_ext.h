/*
*********************************************************************************************************
*                                     MEMORY PARTITION DATA STRUCTURES
*********************************************************************************************************
*/

#define INT32U 	unsigned int
#define	INT16U  unsigned short int
#define INT8U   unsigned char

#define OS_NO_ERR                 0
#define OS_MEM_INVALID_PART     110
#define OS_MEM_INVALID_BLKS     111
#define OS_MEM_INVALID_SIZE     112
#define OS_MEM_NO_FREE_BLKS     113
#define OS_MEM_FULL             114
#define OS_MEM_INVALID_PBLK     115
#define OS_MEM_INVALID_PMEM     116
#define OS_MEM_INVALID_PDATA    117
#define OS_MEM_INVALID_ADDR     118

#define	OS_MEM_EN   1
#define	OS_MAX_MEM_PART    10
#define  OS_GLOBALS  1
#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#define    OS_ENTER_CRITICAL()   do { } while(0)                /*TO DO */
#define    OS_EXIT_CRITICAL()    do { } while(0) 				/*TO DO*/


struct os_mem {                       /* MEMORY CONTROL BLOCK                                         */
    void   *osmem_addr;                 /* Pointer to beginning of memory partition                     */
    void   *osmem_free_list;             /* Pointer to list of free memory blocks                        */
    INT32U  osmem_blk_size;              /* Size (in bytes) of each block of memory                      */
    INT32U  osmem_nblks;                /* Total number of blocks in this partition                     */
    INT32U  osmem_nfree;                /* Number of memory blocks remaining in this partition          */
};

struct os_mem_data {
    void   *os_addr;                    /* Pointer to the beginning address of the memory partition     */
    void   *os_free_list;                /* Pointer to the beginning of the free list of memory blocks   */
    INT32U  os_blk_size;                 /* Size (in bytes) of each memory block                         */
    INT32U  os_nblks;                   /* Total number of blocks in the partition                      */
    INT32U  os_nfree;                   /* Number of memory blocks free                                 */
    INT32U  os_nused;                   /* Number of memory blocks used                                 */
} ;

struct  os_mem           *osmem_free_list;            /* Pointer to free list of memory partitions       */
struct  os_mem            osmem_tbl[OS_MAX_MEM_PART];/* Storage for memory partition manager            */