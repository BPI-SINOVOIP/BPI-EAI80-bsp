/*-------------------------------------------------------------------------

		FileName    :dlmalloc_cfg.h
		Description :Configuration of Dlmalloc.

-------------------------------------------------------------------------*/
#ifndef __DLMALLOC_CFG_H__
#define __DLMALLOC_CFG_H__

typedef unsigned int size_m;

//use bitmap 
#define MALLOC_DEBUG_OPTION_FOOTERS	(0x8)//with footer
#define MALLOC_DEBUG_OPTION_CHECK	(0x4)//do the full check(enable DEBUG)
#define MALLOC_DEBUG_OPTION_SECURE	(0x2)//address check
#define MALLOC_DEBUG_OPTION_TRACE	(0x1)//only TRACE(whatever the debug-mode is configured, trace will be disabled when compiled in release configration)
#define MALLOC_DEBUG_OPTION_DIS		(0x0)//no footer, no assert, no check , no trace, 

#define __MALLOC_DEBUG_MODE			(MALLOC_DEBUG_OPTION_FOOTERS | MALLOC_DEBUG_OPTION_CHECK |\
									 MALLOC_DEBUG_OPTION_SECURE | MALLOC_DEBUG_OPTION_TRACE)

#ifndef NDEBUG
#define MALLOC_DEBUG_MODE			(__MALLOC_DEBUG_MODE | MALLOC_DEBUG_OPTION_TRACE)
#else
//when the module is stablized, we can disable debug-mode
#define MALLOC_DEBUG_MODE			(__MALLOC_DEBUG_MODE & (~MALLOC_DEBUG_OPTION_TRACE))
#endif

/*------------- definitions -------------*/

#undef WIN32
#undef _WIN32
/*
WIN32                    default: defined if _WIN32 defined
  Defining WIN32 sets up defaults for MS environment and compilers.
  Otherwise defaults are for unix. Beware that there seem to be some
  cases where this malloc might not be a pure drop-in replacement for
  Win32 malloc: Random-looking failures from Win32 GDI API's (eg;
  SetDIBits()) may be due to bugs in some video driver implementations
  when pixel buffers are malloc()ed, and the region spans more than
  one VirtualAlloc()ed region. Because dlmalloc uses a small (64Kb)
  default granularity, pixel buffers may straddle virtual allocation
  regions more often than when using the Microsoft allocator.  You can
  avoid this by using VirtualAlloc() and VirtualFree() for all pixel
  buffers rather than using malloc().  If this is not possible,
  recompile this malloc with a larger DEFAULT_GRANULARITY. */


#define MALLOC_ALIGNMENT		(size_m)8
/*
MALLOC_ALIGNMENT         default: (size_m)8
  Controls the minimum alignment for malloc'ed chunks.  It must be a
  power of two and at least 8, even on machines for which smaller
  alignments would suffice. It may be defined as larger than this
  though. Note however that code and data structures are optimized for
  the case of 8-byte alignment.*/


#define MSPACES                 1
/*
MSPACES                  default: 0 (false)
  If true, compile in support for independent allocation spaces.
  This is only supported if HAVE_MMAP is true.*/


#define ONLY_MSPACES            1
/*
ONLY_MSPACES             default: 0 (false)
  If true, only compile in mspace versions, not regular versions.*/


#define USE_LOCKS               2//use a semaphore as a spinlock
/*
USE_LOCKS                default: 0 (false)
  Causes each call to each public routine to be surrounded with
  pthread or WIN32 mutex lock/unlock. (If set true, this can be
  overridden on a per-mspace basis for mspace versions.) If set to a
  non-zero value other than 1, locks are used, but their
  implementation is left out, so lock functions must be supplied manually.*/


#define USE_SPIN_LOCKS			0
/*
USE_SPIN_LOCKS           default: 1 if USE_LOCKS and on x86 using gcc or MSC
  If true, uses custom spin locks for locking. This is currently
  supported only for x86 platforms using gcc or recent MS compilers.
  Otherwise, posix locks or win32 critical sections are used.*/


#if (MALLOC_DEBUG_MODE & MALLOC_DEBUG_OPTION_FOOTERS)
#define FOOTERS_4_OVERWRITE		1
#define FOOTERS_4_MEMLEAK		2
#define FOOTERS                 FOOTERS_4_OVERWRITE
#if (FOOTERS_4_MEMLEAK == FOOTERS)
static int _uid;
#define MALLOC_USERID_GUID		(0x53253223)
#define INVALID_MALLOC_USER_ID	(0)
#endif
/*Note:	In order to debug memory leak, I overlaid the FOOTERS field to contain a USERID.
		when FOOTERS is defined to 1, it behaves regularly.
		when FOOTERS is defined to a larger value, it contain a USERID and the FOOTER check is disabled.
*/
#else
#define FOOTERS					0
#endif
/*
FOOTERS                  default: 0
  If true, provide extra checking and dispatching by placing
  information in the footers of allocated chunks. This adds
  space and time overhead.*/


#if (MALLOC_DEBUG_MODE & MALLOC_DEBUG_OPTION_SECURE)//only when the stablization of whole system
													//is proved, allocator can run in INSECURE
#define INSECURE                0
#else
#define INSECURE				1
#endif
/*
INSECURE                 default: 0
  If true, omit checks for usage errors and heap space overwrites.*/


#define USE_DL_PREFIX			//this is defined to avoid override of standard lib malloc functions
/*
USE_DL_PREFIX            default: NOT defined
  Causes compiler to prefix all public routines with the string 'dl'.
  This can be useful when you only want to use this malloc in one part
  of a program, using your regular system malloc elsewhere.*/


#define ABORT					do{ASSERT(FALSE);}while(0)
/*
ABORT                    default: defined as abort()
  Defines how to abort on failed checks.  On most systems, a failed
  check cannot die with an "assert" or even print an informative
  message, because the underlying print routines in turn call malloc,
  which will fail again.  Generally, the best policy is to simply call
  abort(). It's not very useful to do more than this because many
  errors due to overwriting will show up as address faults (null, odd
  addresses etc) rather than malloc-triggered checks, so will also
  abort.  Also, most compilers know that abort() does not return, so
  can better optimize code conditionally calling it.*/


#define PROCEED_ON_ERROR		0
/*
PROCEED_ON_ERROR           default: defined as 0 (false)
  Controls whether detected bad addresses cause them to bypassed
  rather than aborting. If set, detected bad arguments to free and
  realloc are ignored. And all bookkeeping information is zeroed out
  upon a detected overwrite of freed heap space, thus losing the
  ability to ever return it from malloc again, but enabling the
  application to proceed. If PROCEED_ON_ERROR is defined, the
  static variable malloc_corruption_error_count is compiled in
  and can be examined to see if errors have occurred. This option
  generates slower code than the default abort policy.*/


#if (MALLOC_DEBUG_MODE & MALLOC_DEBUG_OPTION_CHECK)
#define DEBUG					1
#else
#undef	DEBUG
#endif
/*
DEBUG                    default: NOT defined
  The DEBUG setting is mainly intended for people trying to modify
  this code or diagnose problems when porting to new platforms.
  However, it may also be able to better isolate user errors than just
  using runtime checks.  The assertions in the check routines spell
  out in more detail the assumptions and invariants underlying the
  algorithms.  The checking is fairly extensive, and will slow down
  execution noticeably. Calling malloc_stats or mallinfo with DEBUG
  set will attempt to check every non-mmapped allocated and free chunk
  in the course of computing the summaries.*/
  
#define ABORT_ON_ASSERT_FAILURE	1
/*
ABORT_ON_ASSERT_FAILURE   default: defined as 1 (true)
  Debugging assertion failures can be nearly impossible if your
  version of the assert macro causes malloc to be called, which will
  lead to a cascade of further failures, blowing the runtime stack.
  ABORT_ON_ASSERT_FAILURE cause assertions failures to call abort(),
  which will usually make debugging easier.*/


#define MALLOC_FAILURE_ACTION	//do{TRACE("malloc fail.\n");ASSERT(FALSE);}while(0)//to do
/*
MALLOC_FAILURE_ACTION     default: sets errno to ENOMEM, or no-op on win32
  The action to take before "return 0" when malloc fails to be able to
  return memory because there is none available.*/


#define HAVE_MORECORE			0
/*
HAVE_MORECORE             default: 1 (true) unless win32 or ONLY_MSPACES
  True if this system supports sbrk or an emulation of it.*/


#define MORECORE(n)				//do not support morecore
/*
MORECORE                  default: sbrk
  The name of the sbrk-style system routine to call to obtain more
  memory.  See below for guidance on writing custom MORECORE
  functions. The type of the argument to sbrk/MORECORE varies across
  systems.  It cannot be size_m, because it supports negative
  arguments, so it is normally the signed type of the same width as
  size_m (sometimes declared as "intptr_t").  It doesn't much matter
  though. Internally, we only call it with arguments less than half
  the max value of a size_m, which should work across all reasonable
  possibilities, although sometimes generating compiler warnings.  */


#define MORECORE_CONTIGUOUS		0//MORECORE is not supported
/*
MORECORE_CONTIGUOUS       default: 1 (true) if HAVE_MORECORE
  If true, take advantage of fact that consecutive calls to MORECORE
  with positive arguments always return contiguous increasing
  addresses.  This is true of unix sbrk. It does not hurt too much to
  set it true anyway, since malloc copes with non-contiguities.
  Setting it false when definitely non-contiguous saves time
  and possibly wasted space it would take to discover this though.*/


#define MORECORE_CANNOT_TRIM	//MORECORE is not supported, it is not necessary to check whether to TRIM
/*
MORECORE_CANNOT_TRIM      default: NOT defined
  True if MORECORE cannot release space back to the system when given
  negative arguments. This is generally necessary only if you are
  using a hand-crafted MORECORE function that cannot handle negative
  arguments.*/

#define NO_SEGMENT_TRAVERSAL	1//MORECORE is not supported, it is not necessary to check whether to traverse segment
/*
NO_SEGMENT_TRAVERSAL       default: 0
  If non-zero, suppresses traversals of memory segments
  returned by either MORECORE or CALL_MMAP. This disables
  merging of segments that are contiguous, and selectively
  releasing them to the OS if unused, but bounds execution times.*/


#define HAVE_MMAP				0
/*
HAVE_MMAP                 default: 1 (true)
  True if this system supports mmap or an emulation of it.  If so, and
  HAVE_MORECORE is not true, MMAP is used for all system
  allocation. If set and HAVE_MORECORE is true as well, MMAP is
  primarily used to directly allocate very large blocks. It is also
  used as a backup strategy in cases where MORECORE fails to provide
  space from system. Note: A single call to MUNMAP is assumed to be
  able to unmap memory that may have be allocated using multiple calls
  to MMAP, so long as they are adjacent.*/


#define HAVE_MREMAP				0
/*
HAVE_MREMAP               default: 1 on linux, else 0
  If true realloc() uses mremap() to re-allocate large blocks and
  extend or shrink allocation spaces.*/


#define MMAP_CLEARS				0//mmap is not supported, it is not necessary to check.
/*
MMAP_CLEARS               default: 1 except on WINCE.
  True if mmap clears memory so calloc doesn't need to. This is true
  for standard unix mmap using /dev/zero and on WIN32 except for WINCE.*/


#define USE_BUILTIN_FFS			0//
#define USE_ASSEMBLY_CLZ		0//use ARM assembly instruction CLZ(count leading zero) to calcuate the bin index
								 //ARM architecture version 5 and above support CLZ instruction
								 //need conform
/*
USE_BUILTIN_FFS            default: 0 (i.e., not used)
  Causes malloc to use the builtin ffs() function to compute indices.
  Some compilers may recognize and intrinsify ffs to be faster than the
  supplied C version. Also, the case of x86 using gcc is special-cased
  to an asm instruction, so is already as fast as it can be, and so
  this setting has no effect. Similarly for Win32 under recent MS compilers.
  (On most x86s, the asm version is only slightly faster than the C version.)*/


#define malloc_getpagesize		0//not support
/*
malloc_getpagesize         default: derive from system includes, or 4096.
  The system page size. To the extent possible, this malloc manages
  memory from the system in page-size units.  This may be (and
  usually is) a function rather than a constant. This is ignored
  if WIN32, where page size is determined using getSystemInfo during
  initialization.*/


#define USE_DEV_RANDOM			0
/*
USE_DEV_RANDOM             default: 0 (i.e., not used)
  Causes malloc to use /dev/random to initialize secure magic seed for
  stamping footers. Otherwise, the current time is used.*/


#define NO_MALLINFO				0
/*
NO_MALLINFO                default: 0
  If defined, don't compile "mallinfo". This can be a simple way
  of dealing with mismatches between system declarations and
  those in this file.*/


#define MALLINFO_FIELD_TYPE		size_m
/*
MALLINFO_FIELD_TYPE        default: size_m
  The type of the fields in the mallinfo struct. This was originally
  defined as "int" in SVID etc, but is more usefully defined as
  size_m. The value is used only if  HAVE_USR_INCLUDE_MALLOC_H is not set*/


#undef	REALLOC_ZERO_BYTES_FREES
/*
REALLOC_ZERO_BYTES_FREES    default: not defined
  This should be set if a call to realloc with zero bytes should
  be the same as a call to free. Some people think it should. Otherwise,
  since this malloc returns a unique pointer for malloc(0), so does
  realloc(p, 0).*/


/*			additional lack of header file				*/
//#define LACKS_TIME_H			//time is negative. use rand() for instead.
#define time(p)					rand()

#define LACKS_STDIO_H
#define m_fprintf				if(0)uprint
		/*TRACE((fmt), __VA_ARGS__)//m_fprintf is negative, use trace for instead.*/


/*
DEFAULT_GRANULARITY        default: page size if MORECORE_CONTIGUOUS,
                                system_info.dwAllocationGranularity in WIN32,
                                otherwise 64K.
      Also settable using mallopt(M_GRANULARITY, x)
  The unit for allocating and deallocating memory from the system.  On
  most systems with contiguous MORECORE, there is no reason to
  make this more than a page. However, systems with MMAP tend to
  either require or encourage larger granularities.  You can increase
  this value to prevent system allocation functions to be called so
  often, especially if they are slow.  The value must be at least one
  page and must be a power of two.  Setting to 0 causes initialization
  to either page size or win32 region size.  (Note: In previous
  versions of malloc, the equivalent of this option was called
  "TOP_PAD")*/

/*
DEFAULT_TRIM_THRESHOLD    default: 2MB
      Also settable using mallopt(M_TRIM_THRESHOLD, x)
  The maximum amount of unused top-most memory to keep before
  releasing via malloc_trim in free().  Automatic trimming is mainly
  useful in long-lived programs using contiguous MORECORE.  Because
  trimming via sbrk can be slow on some systems, and can sometimes be
  wasteful (in cases where programs immediately afterward allocate
  more large chunks) the value should be high enough so that your
  overall system performance would improve by releasing this much
  memory.  As a rough guide, you might set to a value close to the
  average size of a process (program) running on your system.
  Releasing this much memory would allow such a process to run in
  memory.  Generally, it is worth tuning trim thresholds when a
  program undergoes phases where several large chunks are allocated
  and released in ways that can reuse each other's storage, perhaps
  mixed with phases where there are no such chunks at all. The trim
  value must be greater than page size to have any useful effect.  To
  disable trimming completely, you can set to MAX_SIZE_T. Note that the trick
  some people use of mallocing a huge space and then freeing it at
  program startup, in an attempt to reserve system memory, doesn't
  have the intended effect under automatic trimming, since that memory
  will immediately be returned to the system.*/


/*
DEFAULT_MMAP_THRESHOLD       default: 256K
      Also settable using mallopt(M_MMAP_THRESHOLD, x)
  The request size threshold for using MMAP to directly service a
  request. Requests of at least this size that cannot be allocated
  using already-existing space will be serviced via mmap.  (If enough
  normal freed space already exists it is used instead.)  Using mmap
  segregates relatively large chunks of memory so that they can be
  individually obtained and released from the host system. A request
  serviced through mmap is never reused by any other request (at least
  not directly; the system may just so happen to remap successive
  requests to the same locations).  Segregating space in this way has
  the benefits that: Mmapped space can always be individually released
  back to the system, which helps keep the system level memory demands
  of a long-lived program low.  Also, mapped memory doesn't become
  `locked' between other chunks, as can happen with normally allocated
  chunks, which means that even trimming via malloc_trim would not
  release them.  However, it has the disadvantage that the space
  cannot be reclaimed, consolidated, and then used to service later
  requests, as happens with normal chunks.  The advantages of mmap
  nearly always outweigh disadvantages for "large" chunks, but the
  value of "large" may vary across systems.  The default is an
  empirically derived value that works well in most systems. You can
  disable mmap by setting to MAX_SIZE_T.*/


/*
MAX_RELEASE_CHECK_RATE   default: 4095 unless not HAVE_MMAP
  The number of consolidated frees between checks to release
  unused segments when freeing. When using non-contiguous segments,
  especially with multiple mspaces, checking only for topmost space
  doesn't always suffice to trigger trimming. To compensate for this,
  free() will, with a period of MAX_RELEASE_CHECK_RATE (or the
  current number of segments, if greater) try to release unused
  segments to the OS when freeing chunks that result in
  consolidation. The best value for this parameter is a compromise
  between slowing down frees with relatively costly checks that
  rarely trigger versus holding on to unused memory. To effectively
  disable, set to MAX_SIZE_T. This may lead to a very slight speed
  improvement at the expense of carrying around more memory.
*/
/*-----------------------------------------------------------------------*/
#define FORCEINLINE				__inline

/*-------------------------------------------------------------------------
		addtional defines for the costumization
-------------------------------------------------------------------------*/
#define DESTROY_STATE_AND_PARAM	1

#define NO_MALLOC_STATS 		1

/*
	Changes:
	disable all the system alloc
	realloc: using afresh_realloc. using the first six words in smallbins and exts to STORE_PAYLOAD
	malloc_large : when finding the current size , do not go leftmost.
	make the internal_malloc and internal_free out of spinlock.
	make lock more porpare.
	check m not only when footer but in the case of secure and also	use extp to save m pointer as a securty check.
	none mspace functions are totally obsoleted, there will be mass of errors if simpliy enable them.
*/


#endif //__MALLOC_CFG_H__

