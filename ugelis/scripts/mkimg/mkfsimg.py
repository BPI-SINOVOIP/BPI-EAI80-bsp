#!/usr/bin/env python3
import os
import sys
import getopt
import datetime
import argparse
import configparser

# DBCS code ranges and SBCS upper conversion tables
_CODE_PAGE	=936
_USE_LFN    =1
_MAX_LFN	=255

# File attribute bits (internal use)
AM_VOL		=0x08	# Volume label
AM_LFN		=0x0F	# LFN entry
AM_MASK		=0x3F	# Mask of defined bits
# File attribute bits for directory entry (FILINFO.fattrib)
AM_RDO	=0x01	# Read only
AM_HID	=0x02	# Hidden
AM_SYS	=0x04	# System
AM_DIR	=0x10	# Directory
AM_ARC	=0x20	# Archive

# File access control and file status flags (internal use)
FA_SEEKEND	=0x20	# Seek to end of the file on file open
FA_MODIFIED	=0x40	# File has been modified
FA_DIRTY	=0x80	# FIL.buf[] needs to be written-back

# Name status flags
NSFLAG		=11		# Index of name status byte in fn[]
NS_LOSS		=0x01	# Out of 8.3 format
NS_LFN		=0x02	# Force to create LFN entry
NS_LAST		=0x04	# Last segment
NS_BODY		=0x08	# Lower case flag (body)
NS_EXT		=0x10	# Lower case flag (ext)
NS_DOT		=0x20	# Dot entry
NS_NOLFN	=0x40	# Do not find LFN
NS_NONAME	=0x80	# Not followed

# Limits and boundaries (differ from specs but correct for real DOS/Windows)
MAX_FAT12	=0xFF5			# Maximum number of FAT12 clusters
MAX_FAT16	=0xFFF5			# Maximum number of FAT16 clusters
MAX_FAT32	=0xFFFFFF5		# Maximum number of FAT32 clusters
MAX_EXFAT	=0x7FFFFFFD		# Maximum number of exFAT clusters (limited by implementation)
MAX_DIR		=0x200000		# Maximum size of FAT directory
MAX_DIR_EX	=0x10000000		# Maximum size of exFAT directory

# FatFs refers the members in the FAT structures as byte array instead of
# structure members because the structure is not binary compatible between
# different platforms

BS_JmpBoot			=0		# x86 jump instruction (3-byte)
BS_OEMName			=3		# OEM name (8-byte)
BPB_BytsPerSec		=11		# Sector size [byte] (uint16_t)
BPB_SecPerClus		=13		# Cluster size [sector] (uint8_t)
BPB_RsvdSecCnt		=14		# Size of reserved area [sector] (uint16_t)
BPB_NumFATs			=16		# Number of FATs (uint8_t)
BPB_RootEntCnt		=17		# Size of root directory area for FAT12/16 [entry] (uint16_t)
BPB_TotSec16		=19		# Volume size (16-bit) [sector] (uint16_t)
BPB_Media			=21		# Media descriptor byte (uint8_t)
BPB_FATSz16			=22		# FAT size (16-bit) [sector] (uint16_t)
BPB_SecPerTrk		=24		# Track size for int13h [sector] (uint16_t)
BPB_NumHeads		=26		# Number of heads for int13h (uint16_t)
BPB_HiddSec			=28		# Volume offset from top of the drive (uint32_t)
BPB_TotSec32		=32		# Volume size (32-bit) [sector] (uint32_t)
BS_DrvNum			=36		# Physical drive number for int13h (uint8_t)
BS_NTres			=37		# Error flag (uint8_t)
BS_BootSig			=38		# Extended boot signature (uint8_t)
BS_VolID			=39		# Volume serial number (uint32_t)
BS_VolLab			=43		# Volume label string (8-byte)
BS_FilSysType		=54		# File system type string (8-byte)
BS_BootCode			=62		# Boot code (448-byte)
BS_55AA				=510	# Signature word (uint16_t)

BPB_FATSz32			=36		# FAT32: FAT size [sector] (uint32_t)
BPB_ExtFlags32		=40		# FAT32: Extended flags (uint16_t)
BPB_FSVer32			=42		# FAT32: File system version (uint16_t)
BPB_RootClus32		=44		# FAT32: Root directory cluster (uint32_t)
BPB_FSInfo32		=48		# FAT32: Offset of FSINFO sector (uint16_t)
BPB_BkBootSec32		=50		# FAT32: Offset of backup boot sector (uint16_t)
BS_DrvNum32			=64		# FAT32: Physical drive number for int13h (uint8_t)
BS_NTres32			=65		# FAT32: Error flag (uint8_t)
BS_BootSig32		=66		# FAT32: Extended boot signature (uint8_t)
BS_VolID32			=67		# FAT32: Volume serial number (uint32_t)
BS_VolLab32			=71		# FAT32: Volume label string (8-byte)
BS_FilSysType32		=82		# FAT32: File system type string (8-byte)
BS_BootCode32		=90		# FAT32: Boot code (420-byte)

BPB_ZeroedEx		=11		# exFAT: MBZ field (53-byte)
BPB_VolOfsEx		=64		# exFAT: Volume offset from top of the drive [sector] (uint64_t)
BPB_TotSecEx		=72		# exFAT: Volume size [sector] (uint64_t)
BPB_FatOfsEx		=80		# exFAT: FAT offset from top of the volume [sector] (uint32_t)
BPB_FatSzEx			=84		# exFAT: FAT size [sector] (uint32_t)
BPB_DataOfsEx		=88		# exFAT: Data offset from top of the volume [sector] (uint32_t)
BPB_NumClusEx		=92		# exFAT: Number of clusters (uint32_t)
BPB_RootClusEx		=96		# exFAT: Root directory cluster (uint32_t)
BPB_VolIDEx			=100	# exFAT: Volume serial number (uint32_t)
BPB_FSVerEx			=104	# exFAT: File system version (uint16_t)
BPB_VolFlagEx		=106	# exFAT: Volume flags (uint8_t)
BPB_ActFatEx		=107	# exFAT: Active FAT flags (uint8_t)
BPB_BytsPerSecEx	=108	# exFAT: Log2 of sector size in byte (uint8_t)
BPB_SecPerClusEx	=109	# exFAT: Log2 of cluster size in sector (uint8_t)
BPB_NumFATsEx		=110	# exFAT: Number of FATs (uint8_t)
BPB_DrvNumEx		=111	# exFAT: Physical drive number for int13h (uint8_t)
BPB_PercInUseEx		=112	# exFAT: Percent in use (uint8_t)
BPB_RsvdEx			=113	# exFAT: Reserved (7-byte)
BS_BootCodeEx		=120	# exFAT: Boot code (390-byte)

FSI_LeadSig			=0		# FAT32 FSI: Leading signature (uint32_t)
FSI_StrucSig		=484	# FAT32 FSI: Structure signature (uint32_t)
FSI_Free_Count		=488	# FAT32 FSI: Number of free clusters (uint32_t)
FSI_Nxt_Free		=492	# FAT32 FSI: Last allocated cluster (uint32_t)

MBR_Table			=446	# MBR: Offset of partition table in the MBR
SZ_PTE				=16		# MBR: Size of a partition table entry
PTE_Boot			=0		# MBR PTE: Boot indicator
PTE_StHead			=1		# MBR PTE: Start head
PTE_StSec			=2		# MBR PTE: Start sector
PTE_StCyl			=3		# MBR PTE: Start cylinder
PTE_System			=4		# MBR PTE: System ID
PTE_EdHead			=5		# MBR PTE: End head
PTE_EdSec			=6		# MBR PTE: End sector
PTE_EdCyl			=7		# MBR PTE: End cylinder
PTE_StLba			=8		# MBR PTE: Start in LBA
PTE_SizLba			=12		# MBR PTE: Size in LBA

DIR_Name			=0		# Short file name (11-byte)
DIR_Attr			=11		# Attribute (uint8_t)
DIR_NTres			=12		# Lower case flag (uint8_t)
DIR_CrtTime10		=13		# Created time sub-second (uint8_t)
DIR_CrtTime			=14		# Created time (uint32_t)
DIR_LstAccDate		=18		# Last accessed date (uint16_t)
DIR_FstClusHI		=20		# Higher 16-bit of first cluster (uint16_t)
DIR_ModTime			=22		# Modified time (uint32_t)
DIR_FstClusLO		=26		# Lower 16-bit of first cluster (uint16_t)
DIR_FileSize		=28		# File size (uint32_t)
LDIR_Ord			=0		# LFN entry order and LLE flag (uint8_t)
LDIR_Attr			=11		# LFN attribute (uint8_t)
LDIR_Type			=12		# LFN type (uint8_t)
LDIR_Chksum			=13		# Checksum of the SFN entry (uint8_t)
LDIR_FstClusLO		=26		# Must be zero (uint16_t)
XDIR_Type			=0		# Type of exFAT directory entry (uint8_t)
XDIR_NumLabel		=1		# Number of volume label characters (uint8_t)
XDIR_Label			=2		# Volume label (11-uint16_t)
XDIR_CaseSum		=4		# Sum of case conversion table (uint32_t)
XDIR_NumSec			=1		# Number of secondary entries (uint8_t)
XDIR_SetSum			=2		# Sum of the set of directory entries (uint16_t)
XDIR_Attr			=4		# File attribute (uint16_t)
XDIR_CrtTime		=8		# Created time (uint32_t)
XDIR_ModTime		=12		# Modified time (uint32_t)
XDIR_AccTime		=16		# Last accessed time (uint32_t)
XDIR_CrtTime10		=20		# Created time subsecond (uint8_t)
XDIR_ModTime10		=21		# Modified time subsecond (uint8_t)
XDIR_CrtTZ			=22		# Created timezone (uint8_t)
XDIR_ModTZ			=23		# Modified timezone (uint8_t)
XDIR_AccTZ			=24		# Last accessed timezone (uint8_t)
XDIR_GenFlags		=33		# Gneral secondary flags (uint16_t)
XDIR_NumName		=35		# Number of file name characters (uint8_t)
XDIR_NameHash		=36		# Hash of file name (uint16_t)
XDIR_ValidFileSize	=40		# Valid file size (uint64_t)
XDIR_FstClus		=52		# First cluster of the file data (uint32_t)
XDIR_FileSize		=56		# File/Directory size (uint64_t)

SZDIRE				=32		#/* Size of a directory entry
LLEF				=0x40	#/* Last long entry flag in LDIR_Ord
DDEM				=0xE5	#/* Deleted directory entry mark set to DIR_Name[0]
RDDEM				=0x05	#/* Replacement of the character collides with DDEM
ROOT_DIRECTORY_ENTRIES 	=512
DIRECTORY_MAX_ENTERIES  =2048
FILE_SFN			=(0)
FILE_LFN			=(1)

# I wouldn't touch these. First, the partition information.

# Partition status is bootable.
P_STATUS 						=0x80
# File system: 12-bit FAT
P_TYPE_12BIT 					=0x01
# File system: 16-bit FAT
P_TYPE_16BIT 					=0x04
# File system: 16-bit FAT and sectors > 65535
P_TYPE_32MB  					=0x06
# File system: 32-bit FAT
P_TYPE_32BIT 					=0x0C
# File system: ext FAT
P_TYPE_EXFAT  					=0x07

BYTES_PER_SECTOR 				=512
MEDIA_DESCRIPTOR 				=0xf8
FAT_COPIES 						=2
RESERVED_SECTORS 				=1
SECTORS_PER_ROOT_DIRECTORY 		=((ROOT_DIRECTORY_ENTRIES*SZDIRE)//BYTES_PER_SECTOR)

TYPE_FILE						=0
TYPE_ROOT						=1
TYPE_DIR						=2
TYPE_CURDIR						=3
TYPE_PARDIR						=4

ROOT_INDEX						= 0

#variable 类中的变量都是全局变量，用来反映整个文件系统的信息
class cgv:  #class global variable
    SECTORS_PER_TRACK                   = 17
    HEADS                               = 4
    FAT_COPIES                          = 2
    gSectorsPertrack                    = SECTORS_PER_TRACK
    gHeads                              = HEADS
    gTracks                             = 36
    gClusters                           = 0
    gSectorsPerFat                      = 0
    gTotalFileSize                      = 0
    gTotalSectors                       = 0
    gTotalClusters                      = 0
    gFATNums                            = FAT_COPIES
    gp_starting_head                    = 1
    gp_starting_track                   = 0
    gp_starting_sector                  = 1
    gp_starting_absolute_sector         = SECTORS_PER_TRACK
    gp_ending_head                      = HEADS-1
    gp_ending_track                     = 0
    gp_ending_sector                    = SECTORS_PER_TRACK
#Minus 1 for head 0, track 0 (partition table plus empty space)
    gSectors                = 0
    gFType                  = 0
    gSysType                = 0
    gSectorPerFat           = 0
    gBaseSectors            = 0
    gDataSectors            = 0
    gFatSectors             = 0
    gSectorsPerCluster      = 0
    gSectorsResver          = 0
    gBytesPerSector         = 0
    gBytesPerCluster        = 0
    gRootCnt                = 0

    gInputFileCount         = 0
    gInputFileIndex         = 0
    volume_label            = "data"
    gBuffer                 = [0]*BYTES_PER_SECTOR
    gPfat                   = [0]*(gSectorPerFat*BYTES_PER_SECTOR)
    gBasedir                = ' '*50
    gCurdir                 = ' '*50
    gBuffilename            = ' '*50
    directory               = [0]*int(SECTORS_PER_ROOT_DIRECTORY*BYTES_PER_SECTOR)
    #Where to place next input file
    first_available_cluster = 2
    current_path            = ' '*512
    rela_path               = ' '*512
    cmp_flag                = 0
    
#array object 包含每个文件和文件夹的基本信息
class input_file:
    def __init__(self):
        self.fullname = []
        self.dos_filename = [0] * 12
        self.lfnbuf = [0] * 255
        self.base = []
        self.ext = []
        self.filename = ''
        self.size = 0
        self.mtime = 0
        self.starting_cluster = 0
        self.fat_finished = 0
        self.dir_finished = 0
        self.lfn = 0
        self.ending_cluster = 0
        self.size_in_clusters = 0
        self.type = 0
        self.index = 0
        self.parent = 0
        pass

#用来存储文件和文件夹的状态信息
class stat:
    st_dev  = 0
    st_ino  = 0
    st_mode = 0
    st_nlink= 0
    st_uid  = 0
    st_gid  = 0
    st_rdev = 0
    st_size = 0
    st_atime= 0
    st_mtime= 0
    st_ctime= 0

cData = {}  #使用dict存储包含文件信息和文件夹信息的对象

if _CODE_PAGE == 936:        #编码设定
    _DF1S   =	0x81
    _DF1E   =	0xFE
    _DS1S   =	0x40
    _DS1E   =	0x7E
    _DS2S   =	0x80
    _DS2E   =	0xFE

gLfnOfs = [1,3,5,7,9,14,16,18,20,22,24,28,30]   #存放长文件名时地址偏移list

def close_exit(errcode):#错误退出，生成错误日志
    if (cgv.gOutfile != sys.stdout):
        if(cgv.gTotalFileSize):
            print("close_exit gTotalFileSize:%d\n" %cgv.gTotalFileSize)
    exit(errcode)

def usage():#用法提示
    print("Usage:\n"
          "  mkfatimage [-b bsectfile] [{-t tracks | -k Kbytes}]\n"
          "             [-l volume-label] [-f outfile] [-p ] [file...]\n")
    close_exit(1)

def initglobal(): #calculate parameter
    cst = [1, 4, 16, 64, 256, 512, 0]   #Cluster size boundary for FAT12/16 volume (4Ks unit)
    cst32 = [1, 2, 4, 8, 16, 32, 0]     #Cluster size boundary for FAT32 volume (128Ks unit)
    au, b_vol, b_fat, b_data = [0, 0, 0, 0]
    sz_blk = 1

    cgv.gRootCnt = ROOT_DIRECTORY_ENTRIES
    cgv.gFATNums = FAT_COPIES
    cgv.gBytesPerSector = BYTES_PER_SECTOR
    cgv.gTotalSectors = cgv.gTotalFileSize // cgv.gBytesPerSector
    print('gTotalSectors:%s\n' %cgv.gTotalSectors)

    if cgv.gFType == P_TYPE_32BIT:
        cgv.gSysType = 0x0C                   #FAT32bit
    else:
        if cgv.gTotalSectors >= 0x10000:
            cgv.gSysType = 0x06               #FAT12/16
        else:
            cgv.gSysType = 0x04 if cgv.gFType == P_TYPE_16BIT else 0x01
    while 1:
        pau = au
#FAT32
        if cgv.gFType == P_TYPE_32BIT:
            if not pau:
                n = cgv.gTotalSectors // 0x20000                                       #以128k sectors划分Volume
                print("gTotalSectors:%d,n:%d\n" %(cgv.gTotalSectors, n))
                pau = 1
                for i in cst32:
                    if i and i <= n:                                                #根据实际Volume选择pau的值，表示每簇的扇区数
                        pau <<= 1
            n_clst = cgv.gTotalSectors // pau
            sz_fat = (n_clst * 4 + 8 + cgv.gBytesPerSector - 1) // cgv.gBytesPerSector   #FAT表大小
            sz_rsv = 32
            sz_dir = 0
            if n_clst <= MAX_FAT16 or n_clst > MAX_FAT32:
                print("Too few clusters for FAT32:%d\n" %n_clst)
                return(-1)
#FAT12/16
        else:
            if not pau:
                n = cgv.gTotalSectors // 0x1000      #Volume size 4k
                pau = 1
                for i in cst:
                    if i and i <= n:
                        pau <<= 1
            n_clst = cgv.gTotalSectors // pau
            print("pau:%d,n_clst:%d\n" %(pau, n_clst))
            if n_clst > MAX_FAT12:
                n = n_clst * 2 + 4                #FAT size [byte]
            else:
                n = (n_clst * 3 + 1) // 2 + 3
            sz_fat = (n + cgv.gBytesPerSector - 1) // cgv.gBytesPerSector       #FAT size [sector]
            sz_rsv = 1                                                      #reserved sectors
            sz_dir = cgv.gRootCnt * SZDIRE // cgv.gBytesPerSector               #Rootdir size [sector]
        b_fat = b_vol + sz_rsv                                              #FAT 起始地址
        b_data = b_fat + sz_fat * cgv.gFATNums + sz_dir                       #DATA起始位置，簇2

#Align data base to erase block boundary(for flash memory data)
        n = ((b_data + sz_blk - 1) &~ (sz_blk - 1)) - b_data               #Next nearest erase block from current data base,(b_data + sz_blk - 1) &~ (sz_blk - 1)是将b_data的化为sz_blk的最小整数倍，只对sz_blk的值为2的次方才有效
        if cgv.gFType == P_TYPE_32BIT:   #move FAT32 base
            sz_rsv += n
            b_fat += n
        else:                          #Expand FAT12/16 size
            sz_fat += n // cgv.gFATNums
        if cgv.gTotalSectors < b_data + pau * 16 - b_vol:
            print("Too small volume:%d\n" %cgv.gTotalSectors)
            return (-1)
        n_clst = (cgv.gTotalSectors - sz_rsv - sz_fat * cgv.gFATNums - sz_dir) // pau
        if cgv.gFType == P_TYPE_32BIT:
            if n_clst <= MAX_FAT16:
                if au == 0 and (pau / 2) != 0:
                    continue
                print("Too few clusters for FAT32:%d\n" %n_clst)
                return (-1)
        if cgv.gFType == P_TYPE_16BIT:
            if n_clst >= MAX_FAT16:
                if au ==0 and (pau * 2) <= 64:
                    au = pau * 2
                    continue
                if au ==0 and (pau * 2) <= 128:
                    continue
                print("Too many clusters for FAT16:%d\n" %n_clst)
                return (-1)
            if n_clst <= MAX_FAT12:
                if au == 0 and (pau * 2) <= 128:
                    continue
                return (-1)
        if cgv.gFType == P_TYPE_12BIT and n_clst > MAX_FAT12:
            return (-1)
        break
    cgv.gSectorPerFat = sz_fat
    cgv.gBaseSectors = b_vol
    cgv.gDataSectors = b_data
    cgv.gFatSectors = b_fat
    cgv.gTotalClusters = n_clst
    cgv.gSectorsPerCluster = pau
    cgv.gBytesPerCluster = cgv.gSectorsPerCluster * cgv.gBytesPerSector
    cgv.gSectorsResver = sz_rsv
    print("initglobal gSectorsPerCluster:%d\n" %pau)

#need to optimize
def mem_set(dst, val, cnt):
    if type(dst) == str:
        dst = list(dst)
        for i in range(cnt):
            dst.append(val)
            dst[i] = str(dst[i])
        dst = ''.join(dst)
    if type(dst) == list:
        if len(dst) >= cnt:
            for i in range(cnt):
                dst[i] = val
        if len(dst) < cnt:
            for i in range(cnt):
                dst.append(val)
    return dst

def memmove(dst, src, lct, cnt = 0, byte = 2):          #combine memmove and put_word and put_dword
    if isinstance(src, str):
        for i in range(cnt):
            dst[lct+i] = src[i]
    if isinstance(src, list):
        for i in range(cnt):
            dst[lct+i] = src[i]
    if isinstance(src, int):
        if byte == 2:
            dst[lct] = ((src >> 0)&0xff)
            dst[lct + 1] = ((src >> 8) & 0xff)
        if byte == 4 :
            dst[lct] = ((src >> 0)&0xff)
            dst[lct + 1] = ((src >> 8) & 0xff)
            dst[lct + 2] = ((src >> 16) & 0xff)
            dst[lct + 3] = ((src >> 24) & 0xff)


def strcpy(dst, src):
    #need change to int?It works well now
    dst = list(dst)
    src = list(src)
    dst[0:len(src)] = src
    dst = ''.join(dst)
    return dst

def clear_buffer():
#seems there's no different between mem_set and memset
    mem_set(cgv.gBuffer, 0, BYTES_PER_SECTOR)

def fwrite(dst, cnt, f):# dst 为byte类型
    for x in dst:
        if isinstance(x, str):
            x = bytes([ord(x)])
            f.write(x)
        elif isinstance(x, int):
            x = bytes((x,))
            f.write(x)
    if len(dst) < cnt:
        for i in range(cnt - len(dst)):
            f.write(bytes((0x00,)))


def write_buffer():
    with open(cgv.gOutfile, 'ab+') as f:
        fwrite(cgv.gBuffer, BYTES_PER_SECTOR, f)

def put_fat(n ,value):
    if cgv.gFType == P_TYPE_12BIT:
        if n & 1:
            cgv.gPfat[int((n / 2) * 3 + 1)] = (cgv.gPfat[int((n / 2) * 3 + 1)] & ~0xf0) | ((value & 0x0f) << 4)
            cgv.gPfat[int((n / 2) * 3 + 2)] = (value >> 4) & 0xff
        else:
            cgv.gPfat[int((n / 2) * 3)] = value & 0xff
            cgv.gPfat[int((n / 2) * 3 + 1)] = (cgv.gPfat[int(n / 2 * 3 + 1)] & ~0x0f) | ((value >> 8) & 0x0f)
    elif (cgv.gFType == P_TYPE_16BIT) or (cgv.gFType == P_TYPE_32MB):   #制作fat表头F8FFFFFF
        cgv.gPfat[2 * n] = value & 0xff
        cgv.gPfat[2 * n + 1] = value >> 8
    elif cgv.gFType == P_TYPE_32BIT:
        cgv.gPfat[4 * n] = value & 0xff
        cgv.gPfat[4 * n + 1] = (value & 0xff00) >> 8
        cgv.gPfat[4 * n + 2] = (value & 0xff0000) >> 16
        cgv.gPfat[4 * n + 3] = (value & 0xff000000) >> 24
    else:
        print("Error: FAT type %ld unknown\n" %cgv.gFType)
        close_exit(1)

def st(dst, src, lct, len = 2):      #store in little_endian
    if len == 2:
        for i in range(2):
            tmp = src & 0xFF
            dst[lct+i] = tmp
            src >>= 8
#        dst = pack('2B', dst[0], dst[1])
    if len == 4:
        for i in range(4):
            tmp = src & 0xFF
            dst[lct+i] = tmp
            src >>= 8
#        dst = pack('4B', dst[0], dst[1], dst[2], dst[3])
    if len == 8:
        for i in range(8):
            tmp = src & 0xFF
            dst[lct+i] = tmp
            src >>= 8
#        dst = pack('8B', dst[0], dst[1], dst[2], dst[3], dst[4], dst[5], dst[6], dst[7])


def put_lfn(lfn, dir, ord1, sum, offset):
    dir[offset + LDIR_Chksum] = sum
    dir[offset + LDIR_Attr] = AM_LFN
    dir[offset + LDIR_Type] = 0
    st(dir, 0, offset + LDIR_FstClusLO, len=2)
    i = (ord1 - 1) * 13
    wc = 0
    s = 0

    for s in range(13):
        if wc != 0xFFFF:
            if type(lfn[i]) == str:
                wc = ord(lfn[i])
            else:
                wc = lfn[i]
            i += 1
        st(dir, wc, offset + gLfnOfs[s], len=2)
        if wc == 0:
            wc = 0xFFFF
    if wc == 0xFFFF or not lfn[i]:
        ord1 |= LLEF
    dir[offset + LDIR_Ord] = ord1

def IsDBCS1(c):
    return c >= _DF1S and c <= _DF1E

def sfn_compare():
    if cgv.cmp_flag > 1:
        for i in range(cgv.cmp_flag - 1):
            for j in range(i + 1, cgv.cmp_flag):
                if cData[str(i)].dos_filename[0:11] == cData[str(j)].dos_filename[0:11]:
                    cData[str(j)].dos_filename[7] = cData[str(i)].dos_filename[7] + 1
                    
def gen_numname(dst, src, lfn, seq):
    ns = [0]*8
    memmove(dst, src, 0, 11)
    if seq > 5:
        sr = seq
        for wc in lfn:
            for i in range(16):
                sr = sr << 1 + wc & 1
                wc >>= 1
                if sr & 0x10000:
                    sr ^= 0x11021
        seq = sr
    i = 7
    while seq:
        c = seq % 16 + ord('0')
        if c > ord('9'):
            c += 7
        ns[i] = c
        i -= 1
        seq //= 16
    c = seq % 16 + ord('0')
    if c > ord('9'):
        c += 7
        ns -= 1
        ns[i] = c
        seq /= 16
    ns[i] = '~'
    for j in range (i):
        if dst[j] != ' ':
            num = ord(dst[j])
            if IsDBCS1(num):
                if j == i - 1:
                    break
                j += 1
    j += 1
    while j < 8:
        dst[j] = ns[i] if i < 8 else ' '
        j += 1
        i += 1
    sfn_compare()

def sum_sfn(dir):
    sum = 0
    n = 11
    for i in range(n):
        if type(dir[i]) == str:
            sum = (sum >> 1) + (sum << 7) + ord(dir[i])
        if type(dir[i]) == int:
            sum = (sum >> 1) + (sum << 7) + dir[i]
    return sum&0xff

def set_lfn(n, f):  #set lfn entry'n'
    i = n
    num  = 0
    sn = [0]*12
    tm = datetime.datetime.now()
    tmp = f.lfnbuf.copy()
    while tmp[-1] == 0:
        tmp.pop()
    nlen = len(tmp)
    memmove(sn, f.dos_filename, 0, 12)
    if sn[NSFLAG] & NS_LOSS:
        f.dos_filename[NSFLAG] = NS_NOLFN
        gen_numname(f.dos_filename, sn, f.lfnbuf, 1)       #产生数字后缀
        f.dos_filename[NSFLAG] = sn[NSFLAG]
    nent = ((nlen + 12 )// 13) if (sn[NSFLAG] & NS_LFN) else 1
    sum = sum_sfn(f.dos_filename)

    for j in range (nent):
        put_lfn(f.lfnbuf, cgv.directory, nent, sum, i*SZDIRE)
        i += 1
        num += 1
        nent -= 1
    if sn[NSFLAG] & NS_LOSS:
        Y = ((int(tm.strftime('%Y')) - 1980) & 0x7f) << 9
        M = ((int(tm.strftime('%m')) + 1) & 0xf) << 5
        D = int(tm.strftime('%d')) & 0x1f
        ymd = Y | M | D
        memmove(cgv.directory, ymd, i*SZDIRE + XDIR_AccTZ, byte=2)
        H = (int(tm.strftime('%H')) & 0x1f) << 11
        Min = (int(tm.strftime('%M')) & 0x3f) << 5
        S = int(tm.strftime('%S')) & 0x1f
        hms = H | Min | S
        memmove(cgv.directory, hms, i * SZDIRE + XDIR_CrtTZ, byte=2)
        memmove(cgv.directory, f.dos_filename, i * SZDIRE, 11)
        memmove(cgv.directory, f.starting_cluster, i * SZDIRE + DIR_FstClusLO, byte=2)
        if cgv.gFType== P_TYPE_32BIT:
            memmove(cgv.directory, f.starting_cluster>>16, i * SZDIRE + DIR_FstClusHI, byte=2)
        memmove(cgv.directory, f.size, i * SZDIRE + DIR_FileSize, byte=4)
        if f.type == TYPE_DIR:
            cgv.directory[i * SZDIRE + LDIR_Attr] = AM_DIR
        i += 1
        num += 1
    return num

def create_name(filename, type, index):
    cf = 0
#create DOS file name
    base = os.path.basename(filename)
    cData[str(index)].filename = base
    tmp = base
    base = os.path.splitext(tmp)[0]
    ext = os.path.splitext(tmp)[-1][1:]
    baselen = 0
    for w in base:
        if ord(w) > 0x80:
            baselen += 2
        else :
            baselen += 1
    extlen = len(ext)
    cData[str(index)].base = base          #ext 和 base的区别
    cData[str(index)].ext = ext
    if baselen == 0:
        print("%s: File name is not DOS-compatible\n" %filename)
        return
    #decide putting fullname to LFN of SFN
    if (baselen > 0 and baselen < 9) and extlen < 4:
        newbase = ''
        for w in base:
            if ord(w) > 0x80:
                v = w
                v = v.encode('cp936')
                newbase = newbase + chr(v[0])
                newbase = newbase + chr(v[1])
            else:
                w = w.upper()
                newbase = newbase + w
        cData[str(index)].dos_filename = newbase + ' '*(8-len(newbase))+ext.upper()+' '*(3-len(ext))
        cData[str(index)].lfn = FILE_SFN
    else:
        cData[str(index)].lfn = FILE_LFN
#create LFN
        i1 = 0
        for w in cData[str(index)].filename:
    # !_LFN_UNICODE undo
            if ord(w) < 0x80 and w in (r"\"*:<>\?|" or "\x7F"):
                print("%s: Reject illegal characters for LFN \n" %filename)
                return(-1)
            cData[str(index)].lfnbuf[i1] = w
            i1 += 1
# create SFN
        cData[str(index)].dos_filename = mem_set(cData[str(index)].dos_filename, ' ', 11)
        si = 0                             #lfnbuf前面的空格和dot个数
        while cData[str(index)].lfnbuf[0] in ' .':
            si +=1
        if (si):
            cf |= NS_LOSS | NS_LFN                                      #需创建lfn并标志非8.3格式
        # while di and (m[str(index)].lfnbuf[di - 1] != '.'):
        j  = b = 0                                                          #j:filename指针
        ni  = 8
        lf = 0
        v = b'\x00'
        while 1:
            flag = 0
            w = cData[str(index)].lfnbuf[lf]
            if j == 11:
                break
            elif w == ' ' or (w == '.'):
                cf |= NS_LOSS | NS_LFN
                continue
            elif j >= ni or w == '.':
                if ni == 11:
                     cf |= NS_LOSS | NS_LFN
                     break
                if w != '.':
                     cf |= (NS_LOSS | NS_LFN)
                j = 8
                ni = 11
                b <<= 2
                lf = cData[str(index)].lfnbuf.index('.') + 1
                continue
            elif ord(w) >= 0x80:
                v = w
                v = v.encode('cp936')
                flag = 1
                cf |= NS_LFN
            if _DF1S and ord(w) >= 0x100:
                if j >= ni - 1:
                    cf |= NS_LOSS | NS_LFN
                    j = ni
                    continue
                if flag:
                    cData[str(index)].dos_filename[j] = v[0]
                else:
                    cData[str(index)].dos_filename[j] = w
                j += 1
            else:
                if not w or w in "+,;=[]":         #can use find function instead
                    w = '_'
                    cf |= NS_LOSS | NS_LFN
                else:
                    if w.isupper():
                        b |= 2         #大小写标志位
                    elif w.islower():
                        b |= 1
                        w = w.upper()
            if flag:
                cData[str(index)].dos_filename[j] = v[1]
            else:
                cData[str(index)].dos_filename[j] = w
            j += 1
            lf += 1
        if cData[str(index)].dos_filename[0] == DDEM:
            cData[str(index)].dos_filename[0] = RDDEM
        if ni == 8:
            b <<= 2
        if (b & 0x0C) == 0x0C or (b & 0x03) == 0x03:
            cf |= NS_LFN
        if cf & NS_LFN == 0:
            if (b & 0x03) == 0x01:
                cf |= NS_EXT
            if (b & 0x0C) == 0x04:
                cf |= NS_BODY
        cData[str(index)].dos_filename[NSFLAG] = cf
        for a1 in  range(11):
            if isinstance(cData[str(index)].dos_filename[a1], int):
                cData[str(index)].dos_filename[a1] = chr(cData[str(index)].dos_filename[a1])
        return 0

#put input file information to array
def add_input_file(filename, type, index, parent):
    cData[str(index)] = input_file()
    if index >= DIRECTORY_MAX_ENTERIES - 1:
        print("mkfatimage: Root directory full!\n")
        return
    create_name(filename, type, index)
    cData[str(index)].dir_finished      = 0
    cData[str(index)].fat_finished  	= 0
    cData[str(index)].fullname 		    = filename
    cData[str(index)].type				= type
    cData[str(index)].index			    = index

    stat = os.stat(filename)
    try:
        stat != 0
    except FileNotFoundError:
        print("%s: %s\n" %(filename, FileNotFoundError))

    if type == TYPE_FILE:
        cData[str(index)].size = stat.st_size
        cData[str(index)].mtime = stat.st_mtime
        if cData[str(index)].size == 0:
            cData[str(index)].size_in_clusters = 0
            cData[str(index)].starting_cluster = cData[str(index)].ending_cluster = 0
        else:
            cData[str(index)].size_in_clusters = (stat.st_size+cgv.gBytesPerCluster - 1) // cgv.gBytesPerCluster
            cData[str(index)].starting_cluster = cgv.first_available_cluster
            cData[str(index)].ending_cluster = cData[str(index)].starting_cluster + cData[str(index)].size_in_clusters - 1
            cgv.first_available_cluster += cData[str(index)].size_in_clusters
    elif type == TYPE_DIR:
        cData[str(index)].size = SECTORS_PER_ROOT_DIRECTORY*BYTES_PER_SECTOR
        cData[str(index)].mtime = stat.st_mtime
        cData[str(index)].size_in_clusters = (cData[str(index)].size + cgv.gBytesPerCluster - 1) // cgv.gBytesPerCluster
        cData[str(index)].starting_cluster = cgv.first_available_cluster
        cData[str(index)].ending_cluster = cData[str(index)].starting_cluster + cData[str(index)].size_in_clusters - 1
        cgv.first_available_cluster += cData[str(index)].size_in_clusters

def put_directory(n, f):
    if f.lfn == FILE_LFN:
        return set_lfn(n, f)
    else:
        tm = datetime.datetime.now()
        Y = ((int(tm.strftime('%Y')) - 1980) & 0x7f) << 9
        M = ((int(tm.strftime('%m')) + 1) & 0xf) << 5
        D = int(tm.strftime('%d')) & 0x1f
        ymd = Y | M | D
        memmove(cgv.directory, ymd, n * SZDIRE + XDIR_AccTZ, byte=2)
        H = (int(tm.strftime('%H')) & 0x1f) << 11
        Min = (int(tm.strftime('%M')) & 0x3f) << 5
        S = int(tm.strftime('%S')) & 0x1f
        hms = H | Min | S
        memmove(cgv.directory, hms, n * SZDIRE + XDIR_CrtTZ, byte=2)
        memmove(cgv.directory, f.dos_filename, n * SZDIRE, 11)
        memmove(cgv.directory, f.starting_cluster, n * SZDIRE + DIR_FstClusLO, byte=2)
        if cgv.gFType == P_TYPE_32BIT:
            memmove(cgv.directory, f.starting_cluster >> 16, n * SZDIRE + DIR_FstClusHI, byte=2)
        memmove(cgv.directory, f.size, n * SZDIRE + DIR_FileSize, byte=4)
        if f.type == TYPE_DIR:
            cgv.directory[n * SZDIRE + LDIR_Attr] = AM_DIR
        return 1

def fatscandir(dir, index, parent):
    if not os.path.exists(dir):
        print("can not open dir:%s\n" %dir)
        return(-1)
    os.chdir(dir)
    if index != ROOT_INDEX:
        cgv.gCurdir = os.path.split(os.path.realpath(__file__))[0]
        if not os.path.exists(cgv.gCurdir):
            print("Error: get cwd error: %s\n" )
            close_exit(1)
        add_input_file(cgv.gCurdir, TYPE_DIR, index, parent)
        cgv.gInputFileIndex += 1
        cgv.gInputFileCount += 1
    else:
        print("root name:%s/ \n" %cgv.gBasedir)
    fp = []
    fp.append(dir)
    pa = 0
    i = 0
    flag = {dir: 0}
    for roots, dirs, files in os.walk(cgv.gCurdir):
        for file in files:
            cgv.gBuffilename = os.path.join(roots, file)
            print(os.path.join(roots, file))
            fpath = os.path.dirname(cgv.gBuffilename)
            add_input_file(cgv.gBuffilename, TYPE_FILE, cgv.gInputFileIndex, index)
            if fpath in fp:
                cData[str(i)].parent = flag[fpath]
            i += 1
            cgv.gInputFileIndex += 1
            cgv.gInputFileCount += 1
        for dir in dirs:
            if not os.path.split(os.path.realpath(__file__))[0]:
                print("Error: get cwd error")
                close_exit(1)
            cgv.gCurdir = os.path.join(roots, dir)
            fpath = os.path.dirname(cgv.gCurdir)
            add_input_file(cgv.gCurdir, TYPE_DIR, cgv.gInputFileIndex, parent)
            if fpath in fp:
                cData[str(i)].parent = flag[fpath]
            if cgv.gCurdir not in fp:
                flag[cgv.gCurdir] = i
                fp.append(cgv.gCurdir)
            i += 1
            cgv.gInputFileIndex += 1
            cgv.gInputFileCount += 1

    print('InputFileIndex:%s' %cgv.gInputFileIndex)
    print('InputFileCount:%s' %cgv.gInputFileCount)
    os.chdir('..')
    if not os.path.split(os.path.realpath(__file__))[0]:
        # dont know how to use strerror
        print("Error: get cwd error: \n")
        close_exit(1)
    return 0

def mkfatinfo():
    with open(cgv.gOutfile, 'wb') as f:
        f.truncate()    #截断数据，对相同路径下的相同文件进行重复写，而不产生追加的内容，用于测试
    clear_buffer()
    memmove(cgv.gBuffer, "\xEB\xFE\x90" "MSDOS5.0", BS_JmpBoot, 11)
    memmove(cgv.gBuffer, cgv.gBytesPerSector, BPB_BytsPerSec, byte=2)
    cgv.gBuffer[BPB_SecPerClus] = cgv.gSectorsPerCluster
    memmove(cgv.gBuffer, cgv.gSectorsResver, BPB_RsvdSecCnt, byte=2)
    cgv.gBuffer[BPB_NumFATs] = cgv.gFATNums
    memmove(cgv.gBuffer, 0 if cgv.gFType == P_TYPE_32BIT else cgv.gRootCnt, BPB_RootEntCnt, byte=2)
    if cgv.gTotalSectors < 0x10000:
        memmove(cgv.gBuffer, cgv.gTotalSectors, BPB_TotSec16, byte=2)
    else:
        memmove(cgv.gBuffer, cgv.gTotalSectors, BPB_TotSec32, byte=4)
    cgv.gBuffer[BPB_Media] = 0xF8
    memmove(cgv.gBuffer, 63, BPB_SecPerTrk, byte=2)
    memmove(cgv.gBuffer, 255, BPB_NumHeads, byte=2)
    memmove(cgv.gBuffer, cgv.gBaseSectors, BPB_HiddSec, byte=4)
    if cgv.gFType == P_TYPE_32BIT:
        memmove(cgv.gBuffer, 0x12345678, BS_VolID32, byte=4)
        memmove(cgv.gBuffer, cgv.gSectorPerFat, BPB_FATSz32, byte=4)
        memmove(cgv.gBuffer, 2, BPB_RootClus32, byte=4)
        memmove(cgv.gBuffer, 1, BPB_FSInfo32, byte=2)
        memmove(cgv.gBuffer, 6, BPB_BkBootSec32, byte=2)
        cgv.gBuffer[BS_DrvNum32] = 0x80
        cgv.gBuffer[BS_BootSig32] = 0x29
        memmove(cgv.gBuffer, "NO NAME    " "FAT32   ", BS_VolLab32, 19)
    else:
        memmove(cgv.gBuffer, 0x12345678, BS_VolID, byte=4)
        memmove(cgv.gBuffer, cgv.gSectorPerFat, BPB_FATSz16, byte=2)
        cgv.gBuffer[BS_DrvNum] = 0x80
        cgv.gBuffer[BS_BootSig] = 0x29
        memmove(cgv.gBuffer, "NO NAME    " "FAT     ", BS_VolLab, 19)
    if (0):
        cgv.gBuffer[PTE_Boot+MBR_Table] = 0
        cgv.gBuffer[PTE_StHead+MBR_Table] = 1
        cgv.gBuffer[PTE_StSec+MBR_Table] = 1
        cgv.gBuffer[PTE_StCyl+MBR_Table] = 0
        cgv.gBuffer[PTE_System+MBR_Table] = cgv.gSysType
        n = (cgv.gBaseSectors + cgv.gTotalSectors) / (63 * 255)
        cgv.gBuffer[PTE_EdHead+MBR_Table] = 254
        cgv.gBuffer[PTE_EdSec+MBR_Table] = (n >> 2 | 63)
        cgv.gBuffer[PTE_EdCyl+MBR_Table] = n
        memmove(cgv.gBuffer, cgv.gBaseSectors, PTE_StLba+MBR_Table, byte=2)
        memmove(cgv.gBuffer, cgv.gTotalSectors, PTE_SizLba+MBR_Table, byte=2)
    memmove(cgv.gBuffer, 0xAA55, BS_55AA, byte=2)
    write_buffer()
#Create FSINFO record if needed
    if cgv.gFType == P_TYPE_32BIT:
        with open(cgv.gOutfile, 'wb') as f:
            f.seek((cgv.gBaseSectors + 6) * BYTES_PER_SECTOR, 0)
            print('f.tell()', f.tell())
            fwrite(cgv.gBuffer, BYTES_PER_SECTOR, f)
            mem_set(cgv.gBuffer, 0, cgv.gBytesPerSector)
            memmove(cgv.gBuffer, 0x41615252, FSI_LeadSig, byte=4)
            memmove(cgv.gBuffer, 0x61417272, FSI_StrucSig, byte=4)
            memmove(cgv.gBuffer, cgv.gClusters - 1, FSI_Free_Count, byte=4)
            memmove(cgv.gBuffer, 2, FSI_Nxt_Free, byte=4)
            memmove(cgv.gBuffer, 0xAA55, BS_55AA, byte=2)
            f.seek((cgv.gBaseSectors + 7) * BYTES_PER_SECTOR, 0)
            fwrite(cgv.gBuffer, BYTES_PER_SECTOR, f)
            f.seek((cgv.gBaseSectors + 1) * BYTES_PER_SECTOR, 0)
            fwrite(cgv.gBuffer, BYTES_PER_SECTOR, f)
#Write FATs
    cgv.gPfat = [0]*(cgv.gSectorPerFat*BYTES_PER_SECTOR)
    mem_set(cgv.gPfat, 0, len(cgv.gPfat))
    if cgv.gFType == P_TYPE_32BIT:
        put_fat(0, 0xFFFFFFF8)  #Entry 0
        put_fat(1, 0xFFFFFFFF)  #Entry 1
        put_fat(2, 0x0FFFFFFF)  #Entry 2(root directory)
    else:
        put_fat(0, 0xfff8)  #制作FAT表开头
        put_fat(1, 0xffff)
    global cData
    for n in range(cgv.gInputFileCount):  #制作放入FAT表中的文件的开始和结束簇号
        if cData[str(n)].fat_finished != 0:
            continue
        cData[str(n)].fat_finished = 1
        print('%scluster:%s to %s'%(n, cData[str(n)].starting_cluster, cData[str(n)].ending_cluster ))
        for q in range(cData[str(n)].starting_cluster, cData[str(n)].ending_cluster):
            put_fat(q, q + 1)
        if cgv.gFType == P_TYPE_32BIT:
            put_fat(q, 0x0FFFFFFF)
        else:
            put_fat(q + 1, 0xffff)
    with open(cgv.gOutfile, 'ab+') as f:
        for n in range(1, FAT_COPIES + 1):
            fwrite(cgv.gPfat, cgv.gSectorPerFat * BYTES_PER_SECTOR, f)
#Write root directory*
    #directory has been set lenth of 16384bytes
    me = 0
    if len(cgv.volume_label) > 0:# if there's a volume label, add it first
        p = '%-11s' % cgv.volume_label
        for i in range(len(p)):
            cgv.directory[me * 32 + i] = p[i]
        cgv.directory[me * 32 + 11] = 0x08
        me += 1
    for n in range(cgv.gInputFileCount):
        cgv.cmp_flag = n + 1
        if cData[str(n)].dir_finished == 1:
            continue
        if cData[str(n)].parent == 0:
            cData[str(n)].dir_finished = 1
            k = put_directory(me, cData[str(n)])
            me += k
    with open(cgv.gOutfile, 'ab+') as f:
        fwrite(cgv.directory, SECTORS_PER_ROOT_DIRECTORY * BYTES_PER_SECTOR, f)
    return 0

def mkfatdata():
    for n in range(cgv.gInputFileCount):
        if cData[str(n)].type == TYPE_FILE:
            with open(cData[str(n)].fullname, "rb") as f:
                if not f:
                    print("%s: %s\n" %cData[str(n)].fullname)
                    continue
                for i in range(cData[str(n)].size_in_clusters*cgv.gSectorsPerCluster):
                    i += 1
                    clear_buffer()
                    cgv.gBuffer = f.read(BYTES_PER_SECTOR)
                    write_buffer()
        elif cData[str(n)].type == TYPE_DIR:
            mem_set(cgv.directory, 0, len(cgv.directory))
            i = 0
            mem_set(cgv.directory, ' ', 11)
            cgv.directory[DIR_Name] = '.'
            cgv.directory[DIR_Attr] = AM_DIR

            tm = datetime.datetime.now()
            Y = ((int(tm.strftime('%Y')) - 80) & 0x1f) << 9
            M = ((int(tm.strftime('%m')) + 1) & 0xf) << 5
            D = int(tm.strftime('%d')) & 0x1f
            ymd = Y | M | D
            memmove(cgv.directory, ymd, 24, byte=2)
            H = (int(tm.strftime('%H')) & 0x1f) << 11
            Min = (int(tm.strftime('%M')) & 0x3f) << 5
            S = int(tm.strftime('%S')) & 0x1f
            hms = H | Min | S
            memmove(cgv.directory, hms, 22, byte=2)
            memmove(cgv.directory, cData[str(n)].starting_cluster, DIR_FstClusLO, byte=2)
            if cgv.gFType== P_TYPE_32BIT:
                memmove(cgv.directory, cData[str(n)].starting_cluster >> 16, DIR_FstClusHI, byte=2)
            memmove(cgv.directory, cData[str(n)].size, 28, byte=4)
            memmove(cgv.directory, cData[str(n)].mtime, DIR_ModTime, byte=4)

            i += 1
            memmove(cgv.directory, cgv.directory, SZDIRE, SZDIRE)
            cgv.directory[SZDIRE + 1] = '.'
            memmove(cgv.directory, cData[str(cData[str(n)].parent)].starting_cluster, SZDIRE + DIR_FstClusLO, byte=2)
            if cgv.gFType== P_TYPE_32BIT:
                memmove(cgv.directory, (cData[str(cData[str(n)].parent)].starting_cluster) >> 16,SZDIRE + DIR_FstClusHI,byte=2)
            i += 1
            for j in range(cgv.gInputFileCount):
                if cData[str(j)].parent == n:
                    cData[str(j)].dir_finished = 1
                    k = put_directory(i, cData[str(j)])
                    i += k
            with open(cgv.gOutfile, 'ab+') as f:
                fwrite(cgv.directory, SECTORS_PER_ROOT_DIRECTORY*BYTES_PER_SECTOR, f)
    return 0

def main(): #usage:-t fatType -s totalSize(MBytes) -f outputFile -r relativePath -k (break)
#     with open(r"F:\PycharmProjects\mkimg\log.txt", "wb") as l:
#         sys.stderr = l
# # Parse command line
#     if len(sys.argv) <= 1 and l.isatty():
#         usage()
	parser = argparse.ArgumentParser(description='test')
	parser.add_argument("-r", "--resource", nargs=1, required=True, help="resource file name")
	parser.add_argument("-o", "--output", nargs=1, required=True, help="output file name")
	args = parser.parse_args()
	cgv.rela_path = args.resource[0]
	cgv.gOutfile = args.output[0]
	print('resource: %s' %cgv.rela_path)
	print('output: %s'%cgv.gOutfile)
	workPath = os.path.split(os.path.realpath(__file__))[0]
	configPath = 'mkimgconfig.ini'
	configPath = os.path.join(workPath,configPath )
	if os.path.exists(configPath):
		config = configparser.ConfigParser()
		config.read(configPath)
		cgv.gTotalFileSize = int(config.get('mkfsimg', 'size'))
		print("gTotalFileSize:%s\n" % cgv.gTotalFileSize)
		cgv.gFType = int(config.get('mkfsimg', 'type'))
		print("gFType:%s\n" % cgv.gFType)
	else:
		print('need correct parameter!')


	cgv.current_path = os.path.split(os.path.realpath(__file__))[0]
	print('current_path:%s\n' %cgv.current_path)
	if not os.path.exists(cgv.current_path):
		print("Error: get cwd error: %s\n")
		close_exit(1)
	cgv.gTotalFileSize = cgv.gTotalFileSize * 1048576        #from Mb trans to bytes
	#init parameter of img
	initglobal()
	#cgv.gBasedir = cgv.current_path + cgv.rela_path    #相对路径
	cgv.gBasedir = cgv.rela_path                        #修改dts.cmake后的绝对路径
	print(cgv.rela_path)
	#cgv.gBasedir = os.path.join(cgv.current_path, cgv.rela_path)
	#cgv.gBasedir = os.path.abspath(cgv.gBasedir)
	cgv.gCurdir = cgv.gBasedir
	print('basedir%s' %cgv.gBasedir)
	# cgv.gOutfile = cgv.current_path + cgv.gOutfile
	# cgv.gOutfile = os.path.abspath(cgv.gOutfile)


	print("file name:%s \n" %cgv.gBasedir)
	if cgv.gFType == P_TYPE_32BIT:
		cgv.first_available_cluster = 3
	else:
		cgv.first_available_cluster = 2
	if fatscandir(cgv.gBasedir, ROOT_INDEX, ROOT_INDEX) != 0:
		print( "input dir error: Cannot scan directory %s\n" %cgv.gBasedir)
		close_exit(1)
	if mkfatinfo() != 0:
		print("make fat image error: type:%d,gBasedir:%s,total_file_size:%d\n" %(cgv.gFType,cgv.gBasedir,cgv.gTotalFileSize))
		close_exit(1)
	if mkfatdata() != 0:
		print("make fat image data error: type:%d,gBasedir:%s,total_file_size:%d\n" %(cgv.gFType,cgv.gBasedir,cgv.gTotalFileSize))
		close_exit(1)
	close_exit(0)
	return(0)

if __name__ == "__main__":
    main()
