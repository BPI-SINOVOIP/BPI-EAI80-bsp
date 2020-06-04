/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          flash_dev.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/06/27
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/06/27, wizephen.wang, V1.0.0
 *        Initial version.
 */

#ifndef _FLASH_DEV_H_
#define _FLASH_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#if  (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6621) ||\
      defined(CHIP_GM6601B) || defined(CHIP_GM6611B))

/*
 * Flash embedded in GM6601 has totally 64 pages, with 1KB size per page.
 *
 * Both mass-erase and page-erase are allowed to manipulate flash. And, 32Bit or
 * 16Bit data width programmination are also allowed while programming.
 *
 * It takes a maximum of 20us for a completion of data-programing, while it takes
 * more time to complete erase operation (mass-erasing, 10ms; page-erasing, 2ms).
 *
 * Before programing data to a specified target address, it must be ensured that
 * the unique page containing this target address must be erased. If not, unexpected
 * errors may occur while programming.
 */

#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
/* Size per page in bytes */
#define FLASH_PAGE_SIZE                ((uint32_t)0x0400)
/* Pages per flash */
#define FLASH_TOTAL_PAGES              ((uint32_t)0x0040)
/* Size per flash in bytes, equals to FLASH_PAGE_SIZE * FLASH_TOTAL_PAGES */
#define FLASH_TOTAL_SIZE               ((uint32_t)0x10000)
#endif

#if defined(CHIP_GM6621)
/* Size per page in bytes */
#define FLASH_PAGE_SIZE                ((uint32_t)0x0200)
/* Pages per flash */
#define FLASH_TOTAL_PAGES              ((uint32_t)0x0080)
/* Size per flash in bytes, equals to FLASH_PAGE_SIZE * FLASH_TOTAL_PAGES */
#define FLASH_TOTAL_SIZE               ((uint32_t)0x10000)
#endif

#if defined(CHIP_GM6611)
/* Size per page in bytes */
#define FLASH_PAGE_SIZE                ((uint32_t)0x0400)
/* Pages per flash */
#define FLASH_TOTAL_PAGES              ((uint32_t)0x0100)
/* Size per flash in bytes, equals to FLASH_PAGE_SIZE * FLASH_TOTAL_PAGES */
#define FLASH_TOTAL_SIZE               ((uint32_t)0x40000)
#endif

#if defined(CHIP_GM6611B)
/* Size per page in bytes */
#define FLASH_PAGE_SIZE                ((uint32_t)0x0400)
/* Pages per flash */
#define FLASH_TOTAL_PAGES              ((uint32_t)0x0100) //0x0040
/* Size per flash in bytes, equals to FLASH_PAGE_SIZE * FLASH_TOTAL_PAGES */
#define FLASH_TOTAL_SIZE               ((uint32_t)0x40000) //0x10000
#endif


/* Flash logical base address */
#define FLASH_LOGICAL_BASE_ADDRESS     ((uint32_t)0x0000)
/* Flash physical address offset */
#define FLASH_PHYSICAL_ADDRESS_OFFSET  ((uint32_t)0x10000000)

/* Flash physical start address: 0x10000000 */
#define FLASH_START_PHYSICAL_ADDRESS   (FLASH_PHYSICAL_ADDRESS_OFFSET + FLASH_LOGICAL_BASE_ADDRESS)
/* Flash physical end address: 0x10000000 + 64KB */
#define FLASH_END_PHYSICAL_ADDRESS     (FLASH_PHYSICAL_ADDRESS_OFFSET + FLASH_TOTAL_SIZE + FLASH_LOGICAL_BASE_ADDRESS)

/* One bit indicates two pages are locked */
#define FLASH_LOCK_DENSITY             ((uint32_t)(FLASH_PAGE_SIZE * 2))

/* In microsecond, maximum Promgram time is 30 us, with 10 us margin as timeout judgement */
#define FLASH_PROGRAM_MAXTIME          ((int32_t)40)
/* In microsecond, maximum Promgram time is 2.5 ms, with 0.5 ms margin as timeout judgement */
#define FLASH_PAGEERASE_MAXTIME        ((int32_t)3000)
/* In microsecond, maximum Promgram time is 12 ms, with 3 ms margin as timeout judgement */
#define FLASH_MASSERASE_MAXTIME        ((int32_t)15000)

/* Key for unlocking manipulating flash contoller registers */
#define FLASH_UNLOCK_KEY_FOR_MANIPULATION     0xFEDCBA98
#endif


/**
 *Flash Control Register Layout Type Definition
 */
#if defined(CHIP_GM6601)
typedef struct
{
    __IO uint32_t CTRL;         /**< Flash Control Register, offset: 0x00 */
    __IO uint32_t CMD;          /**< Flash Command Register, offset: 0x04 */
    __IO uint32_t ST;           /**< Flash Status Register, offset: 0x08 */
    __IO uint32_t ADDR;         /**< Flash Address Register, offset: 0x0C */
    __IO uint32_t DT;           /**< Flash Data Register, offset: 0x10 */
    __IO uint32_t PRT;          /**< Flash Protection Register, offset: 0x14 */
    __IO uint32_t SYS;          /**< Flash Option Setting Register, offset: 0x18 */
    __IO uint32_t KEY;          /**< Flash Unlock Setting Register, offset: 0x1C */
    __IO uint32_t ANA;          /**< Analog Triming Value Register, offset: 0x20 */
    __IO uint32_t VER0;         /**< Flash Version0 Register, offset: 0x24 */
    __IO uint32_t VER1;         /**< Flash Version1 Register, offset: 0x28 */
} FLASH_Device_T;
#endif

#if defined(CHIP_GM6601B)
typedef struct
{
    __IO uint32_t CTRL;         /**< Flash Control Register, offset: 0x00 */
    __IO uint32_t CMD;          /**< Flash Command Register, offset: 0x04 */
    __IO uint32_t ST;           /**< Flash Status Register, offset: 0x08 */
    __IO uint32_t ADDR;         /**< Flash Address Register, offset: 0x0C */
    __IO uint32_t DT;           /**< Flash Data Register, offset: 0x10 */
    __IO uint32_t PRT;          /**< Flash Protection Register, offset: 0x14 */
    __IO uint32_t SYS;          /**< Flash Option Setting Register, offset: 0x18 */
    __IO uint32_t KEY;          /**< Flash Unlock Setting Register, offset: 0x1C */
    __IO uint32_t ANA;          /**< Analog Triming Value Register, offset: 0x20 */
    __IO uint32_t SECMD;        /**< Flash Security Mode Register, offset: 0x24 */
    __IO uint32_t SECID;        /**< Flash Security Identification, offset: 0x28 */
    uint32_t reserved[7];
    __IO uint32_t VER;          /**< Flash Version Register, offset: 0x48 */
}FLASH_Device_T;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
typedef struct
{
    __IO uint32_t CTRL;         /**< Flash Control Register, offset: 0x00 */
    __IO uint32_t CMD;          /**< Flash Command Register, offset: 0x04 */
    __IO uint32_t ST;           /**< Flash Status Register, offset: 0x08 */
    __IO uint32_t ADDR;         /**< Flash Address Register, offset: 0x0C */
    __IO uint32_t DT;           /**< Flash Data Register, offset: 0x10 */
    __IO uint32_t PRT;          /**< Flash Protection Register, offset: 0x14 */
    __IO uint32_t SYS;          /**< Flash Option Setting Register, offset: 0x18 */
    __IO uint32_t KEY;          /**< Flash Unlock Setting Register, offset: 0x1C */
    __IO uint32_t ANA;          /**< Analog Triming Value Register, offset: 0x20 */
    uint32_t reserved1[3];
    __IO uint32_t FLHIDL;          /**< Flash Chip ID Low Register, offset: 0x30 */
    __IO uint32_t FLHIDH;          /**< Flash Chip ID High Register, offset: 0x34 */
    uint32_t reserved2[2];
    __IO uint32_t FLHCRCE1;     /*!< CRC Error 1 Register,     Address offset: 0x40 */
    __IO uint32_t FLHCRCE2;     /*!< CRC Error 2 Register,     Address offset: 0x44 */
    __IO uint32_t VER;     /*!< CRC Error 3 Register,     Address offset: 0x48 */
} FLASH_Device_T;
#endif

#if defined(CHIP_GM6621)
typedef struct
{
     __IO uint32_t CTRL;         /**< Flash Control Register, offset: 0x00 */
    __IO uint32_t CMD;          /**< Flash Command Register, offset: 0x04 */
    __IO uint32_t ST;           /**< Flash Status Register, offset: 0x08 */
    __IO uint32_t ADDR;         /**< Flash Address Register, offset: 0x0C */
    __IO uint32_t DT;           /**< Flash Data Register, offset: 0x10 */
    __IO uint32_t PRT;          /**< Flash Protection Register, offset: 0x14 */
    __IO uint32_t SYS;          /**< Flash Option Setting Register, offset: 0x18 */
    __IO uint32_t KEY;          /**< Flash Unlock Setting Register, offset: 0x1C */
    __IO uint32_t ANA0;          /**< Analog Triming Value Register0, offset: 0x20 */
    __IO uint32_t ANA1;          /*<Analog Triming Value Register1, offset: 0x24 */
    uint32_t reserved1[2];
    __IO uint32_t FLHIDLL;       /*!< CHIP ID Low Register,     Address offset: 0x30 */
    __IO uint32_t FLHIDLH;       /*!< CHIP ID High Register,     Address offset: 0x34 */
   // uint32_t reserved2[2];
    __IO uint32_t FLHIDHL;      /*!< CHIP ID 95-64 Register,   Address offset: 0x38*/
    __IO uint32_t FLHIDHH;      /*!< CHIP ID 127-96 Register,   Address offset: 0x3c*/
    __IO uint32_t FLHCRCE1;     /*!< CRC Error 1 Register,     Address offset: 0x40 */
    __IO uint32_t FLHCRCE2;     /*!< CRC Error 2 Register,     Address offset: 0x44 */
    __IO uint32_t VER;          /*!< Flash version,     Address offset: 0x48 */
    uint32_t reserved3;         
	__IO uint32_t TRM0;            /*!< Flash timing0 register,     Address offset: 0x50 */
	__IO uint32_t TRM1;            /*!< Flash timing1 register,     Address offset: 0x54 */
	__IO uint32_t TRM2;            /*!< Flash timing2 register,     Address offset: 0x58 */
	uint32_t reserved4; 
	__IO uint32_t SW;               /*!<      Address offset: 0x60 */
} FLASH_Device_T;
#endif


/**
 * Flash program width
 */
typedef enum
{
#if defined(CHIP_GM6601)
    FLASH_PROGRAM_DATA_WIDTH_16BIT  = 0x00,  /**< Programing flash in 16-Bit widith */
    FLASH_PROGRAM_DATA_WIDTH_32BIT           /**< Programing flash in 32-Bit widith */
#endif

#if defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    FLASH_PROGRAM_DATA_WIDTH_32BIT = 0x00    /**< Programing flash in 32-Bit widith */
#endif

#if defined(CHIP_GM6621)
    FLASH_PROGRAM_DATA_WIDTH_32BIT  = 0x00,    /**< Programing flash in 32-Bit widith */
    FLASH_PROGRAM_DATA_WIDTH_16BIT,            /**< Programing flash in 16-Bit widith */
    FLASH_PROGRAM_DATA_WIDTH_8BIT              /**< Programing flash in 8-Bit widith */
#endif

} FLASH_ProgWidth_T;

/**
 * Commands for flash manipulation
 */
typedef enum
{
    FLASH_PROGRAM_CMD     = 0x00,  /**< Program command */
    FLASH_PAGEERASE_CMD   = 0x10,  /**< Page-erasing command */
    FLASH_MASSERASE_CMD   = 0x20   /**< Mass-erasing command */
} FLASH_CmdType_T;

/***************************** Flash Control Register Bitfield ******************************/
#define FLASH_CTRL_LTN                           ((uint32_t) 0x00010000)
#define FLASH_CTRL_LTN_FIELD(x)                  ((uint32_t) ((x) << 16)) /**< Read latency */
#define FLASH_CTRL_EIE                           ((uint32_t) 0x00001000)  /**< Error interrupt enable */
#define FLASH_CTRL_FIE                           ((uint32_t) 0x00000100)  /**< Manipulation interrupt enable */
#define FLASH_CTRL_AEN                           ((uint32_t) 0x00000010)  /**< Flash acceleraion enable */
#define FLASH_CTRL_PGMD                          ((uint32_t) 0x00000001)  /**< Program mode */

/***************************** Flash Status Register Bitfield ******************************/
#define FLASH_ST_ASE                             ((uint32_t) 0x10000000)  /**< Address out of boundary error */
#define FLASH_ST_PVE                             ((uint32_t) 0x01000000)  /**< Program and verify error */
#define FLASH_ST_EPE                             ((uint32_t) 0x00100000)  /**< Erase a protected page error */
#define FLASH_ST_PPE                             ((uint32_t) 0x00010000)  /**< Program a protected page error */
#define FLASH_ST_OMF                             ((uint32_t) 0x00001000)  /**< A mass-erasing finished */
#define FLASH_ST_OEF                             ((uint32_t) 0x00000100)  /**< A page-erasing finished */
#define FLASH_ST_OPF                             ((uint32_t) 0x00000010)  /**< A data-programing finished */
#define FLASH_ST_BSY                             ((uint32_t) 0x00000001)  /**< A mass- or page-erasing or data-programing busy */

/**************************** Flash Command Register Bitfield ******************************/
#define FLASH_CMD_LCK                            ((uint32_t) 0x00000100)  /**< Lock flash registers manipulation */
#define FLASH_CMD_TRG                            ((uint32_t) 0x00000001)  /**< Trigger flash manipulation */

/** Base addresses of Flash Control Registers */
#define FLASH_BASE     (0x40001000)

/** Flash Device Pointer */
#define FLASH          ((FLASH_Device_T *) FLASH_BASE)

#ifdef __cplusplus
}
#endif

#endif /*_FLASH_DEV_H_*/

