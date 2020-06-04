/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_adc.h
 *
 * @author        richard.liu
 *
 * @version       1.0.0
 *
 * @date          2018/02/19
 *
 * @brief         ADC device header file
 *
 * @note
 *    2018/02/19, richard, V1.0.0
 *        Initial version.
 *
 */
#ifndef _ADC_DEV_H_
#define _ADC_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_adc_dev.h"
#include "gm_hal_dma.h"
#include <gm_soc.h>
#include <gm_basic_types.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define ADC_DEV0                              ((ADC_Device_T *)ADC1_BASE) /* ADC device pointer */
#define ADC_DEV1                              ((ADC_Device_T *)ADC2_BASE) /* ADC device pointer */
#define ADC_DEV2                              ((ADC_Device_T *)ADC3_BASE) /* ADC device pointer */

#define ADC1                             	  ((ADC_Device_T *)ADC1_BASE)/* Base addresses of ADC registers */
#define ADC2                                  ((ADC_Device_T *)ADC2_BASE)/* Base addresses of ADC registers */
#define ADC3                                  ((ADC_Device_T *)ADC3_BASE)/* Base addresses of ADC registers */
#define ADC                                   ((ADC_Common_TypeDef *)ADC_COM_BASE) /* Common base addresses of ADC registers*/

#define ADC_READ_FIFO_OFFSET                  0x10   /* ADC read fifo offset To ADC reg base */
#define ADC1_READ_FIFO_PHY_ADDRESS            (uint32_t)((uint32_t)ADC1 + ADC_READ_FIFO_OFFSET) /* ADC read fifo offset physical address */
#define ADC2_READ_FIFO_PHY_ADDRESS            (uint32_t)((uint32_t)ADC2 + ADC_READ_FIFO_OFFSET) /* ADC read fifo offset physical address */
#define ADC3_READ_FIFO_PHY_ADDRESS            (uint32_t)((uint32_t)ADC3 + ADC_READ_FIFO_OFFSET) /* ADC read fifo offset physical address */

#define ADC_CLOCK_GATE_MASK                   (1<<11)/* ADC clock gate maskbit */

/*************** ADC Control Register Bitfiled ******************/
#define ADC_JCOMP_EN_POS                      ((uint32_t) 27) /* ADC uto compare enable on injected channels */
#define ADC_JCOMP_EN                          ((uint32_t) ((uint32_t)1 << ADC_JCOMP_EN_POS))     /* ADC uto compare enable on injected channelse filed */

#define ADC_CTRL_CM_POS                       ((uint32_t) 25) /* ADC compare mode pos */
#define ADC_CTRL_CM                           ((uint32_t) ((uint32_t)3 << ADC_CTRL_CM_POS))     /* ADC compare mode filed */

#define ADC_CTRL_CE_POS                       ((uint32_t) 24) /* ADC auto compare enable pos */
#define ADC_CTRL_CE                           ((uint32_t) ((uint32_t)1 << ADC_CTRL_CE_POS))     /* ADC auto compare enable filed */
#define ADC_CTRL_CMPEN                        ((uint32_t) ((uint32_t)1 << ADC_CTRL_CE_POS))     /* ADC auto compare enable filed */

#define ADC_CTRL_WM_POS                       ((uint32_t) 16) /* ADC work mode pos */
#define ADC_CTRL_WM                           ((uint32_t) ((uint32_t)3 << ADC_CTRL_WM_POS))     /* ADC work mode filed */

#define ADC_CTRL_SINGLE_MODE                  ((uint32_t) ((uint32_t)1 << ADC_CTRL_WM_POS))
#define ADC_CTRL_REPEAT_MODE                  ((uint32_t) ((uint32_t)3 << ADC_CTRL_WM_POS))

#define ADC_COMP_SING_EN_POS                  ((uint32_t) 15) /* ADC Only one selected channel is auto compared pos filed */
#define ADC_COMP_SING_EN                      ((uint32_t) ((uint32_t)1 << ADC_COMP_SING_EN_POS))   /* ADC Only one selected channel is auto compared filed*/

#define ADC_JAUTO_EN_POS                      ((uint32_t) 14) /* ADC Automatic injected group conversion pos filed */
#define ADC_JAUTO_EN                          ((uint32_t) ((uint32_t)1 << ADC_JAUTO_EN_POS))   /* ADC Automatic injected group conversion filed*/

#define ADC_JDCONT_EN_POS                     ((uint32_t) 13) /* ADC Discontinuous mode on injected channels pos filed */
#define ADC_JDCONT_EN                         ((uint32_t) ((uint32_t)1 << ADC_JDCONT_EN_POS))   /* ADC Discontinuous mode on injected channels filed*/

#define ADC_CTRL_DCONTEN_POS                  ((uint32_t) 12) /* ADC discontinuous mode enable pos */
#define ADC_CTRL_DCONTEN                      ((uint32_t) ((uint32_t)1 << ADC_CTRL_DCONTEN_POS))   /* ADC discontinuous mode enable filed */

#define ADC_CTRL_BNIT_POS                     ((uint32_t) 8)  /* ADC resolution mode */
#define ADC_CTRL_BNIT                         ((uint32_t) ((uint32_t)3 << ADC_CTRL_BNIT_POS))   /*  ADC resolution mode filed */

#define ADC_CTRL_LPEN_POS                     ((uint32_t) 3)  /* ADC low power mode enable pos */
#define ADC_CTRL_LPEN                         ((uint32_t) ((uint32_t)1 << ADC_CTRL_LPEN_POS))   /* ADC low power mode enable filed */

#define ADC_CTRL_EN_POS                       ((uint32_t) 0)  /* ADC enable pos */
#define ADC_CTRL_EN                           ((uint32_t) ((uint32_t)1 << ADC_CTRL_EN_POS))       /* ADC enable filed */

/*************** ADC triger register bitfiled ****************************/
#define ADC_TRIG_JCT_POS                      ((uint32_t) 1) /* ADC Software trigger on injected channels pos */
#define ADC_TRIG_JCT                          ((uint32_t) ((uint32_t)1 << ADC_TRIG_JCT_POS))     /* ADC software trigger on injected channels filed */

#define ADC_TRIG_CT_POS                       ((uint32_t) 0) /* ADC software trigger on regular channels  pos */
#define ADC_TRIG_CT                           ((uint32_t) ((uint32_t)1 << ADC_TRIG_CT_POS))     /* ADC software trigger on regular channels  filed */

/*************** ADC interrupt status register bitfiled ******************/
#define ADC_ST_CMI_POS                        ((uint32_t) 28) /* ADC compare match interrupt status pos */
#define ADC_ST_CMI                            ((uint32_t) ((uint32_t)1 << ADC_ST_CMI_POS))      /* ADC compare match interrupt status filed */
#define ADC_ST_DOFI_POS                       ((uint32_t) 24) /* ADC Data Convert Overflow Interrupt Status Pos */
#define ADC_ST_DOFI                           ((uint32_t) ((uint32_t)1 << ADC_ST_DOFI_POS))     /* ADC data convert overflow interrupt status filed */

#define ADC_ST_DOPSI_POS                      ((uint32_t) 20) /* ADC DFIFO prefull interrupt status pos */
#define ADC_ST_DOPSI                          ((uint32_t) ((uint32_t)1 << ADC_ST_DOPSI_POS))     /* ADC DFIFO prefull interrupt status pos*/

#define ADC_ST_DOFSI_POS                      ((uint32_t) 16) /* ADC DFIFO full interrupt status pos */
#define ADC_ST_DOFSI                          ((uint32_t) ((uint32_t)1 << ADC_ST_DOFSI_POS))     /* ADC DFIFO full interrupt status pos*/

#define ADC_ST_JEOS_INT_POS                   ((uint32_t) 13) /* ADC Injected channels end of sequence conversion status pos */
#define ADC_ST_JEOS_INT                       ((uint32_t) ((uint32_t)1 << ADC_ST_JEOS_INT_POS))     /* ADC Injected channels end of sequence conversion status pos*/

#define ADC_ST_EOCS_INT_POS                   ((uint32_t) 12) /* ADC Regular channels end of sequence conversion pos */
#define ADC_ST_EOSI                           ((uint32_t) ((uint32_t)1 << ADC_ST_EOCS_INT_POS))     /* ADC Regular channels end of sequence conversion status filed */
#define ADC_ST_EOCS_INT                       ((uint32_t) ((uint32_t)1 << ADC_ST_EOCS_INT_POS))     /* ADC Regular channels end of sequence conversion status filed */

#define ADC_ST_JEOC_INT_POS                   ((uint32_t) 9)  /* ADC Injected channels end of conversion pos */
#define ADC_ST_JEOC_INT                       ((uint32_t) ((uint32_t)1 << ADC_ST_JEOC_INT_POS))

#define ADC_ST_EOCE_POS                       ((uint32_t) 8)  /* ADC regular channel end of conversion pos */
#define ADC_ST_EOCE                           ((uint32_t) ((uint32_t)1 << ADC_ST_EOCE_POS))

#define ADC_ST_EOC_INT					      ((uint32_t) ((uint32_t)1 << ADC_ST_EOCE_POS))

#define ADC_ST_DFIFO_FULL_POS                 ((uint32_t) 5) /* ADC data FIFO for regular conversions is full pos */
#define ADC_ST_DFIFO_FULL                     ((uint32_t) ((uint32_t)1 << ADC_ST_DFIFO_FULL_POS))     /* ADC data FIFO for regular conversions is full status filed */

#define ADC_ST_DFIFO_EMPTY_POS                ((uint32_t) 4) /* ADC data FIFO for regular conversions is empty pos */
#define ADC_ST_DFIFO_EMPTY                    ((uint32_t) ((uint32_t)1 << ADC_ST_DFIFO_EMPTY_POS))     /* ADC data FIFO for regular conversions is empty status filed */

#define ADC_BUSY_ST_POS                       ((uint32_t) 0) /* ADC  busy status, ADC is busy when in conversion pos */
#define ADC_BUSY_ST                           ((uint32_t) ((uint32_t)1 << ADC_BUSY_ST_POS))     /* ADC busy status, ADC is busy when in conversion status filed */

/***************ADCx Auto Compare Channel Selection  register bitfiled ******************/
#define ADCX_COM_CHN_POS                      ((uint32_t) 16) /* ADC  ADCx Auto Compare Channel Selection Register  pos */
#define ADCX_COM_CHN                          ((uint32_t) ((uint32_t)0X1F << ADC_BUSY_ST_POS))     /* ADC ADCx(0,1,2) Auto Compare Channel Selection Register  filed */

/***************ADCx  Regular Data Result register bitfiled ******************/
#define ADCX_DRST_POS                         ((uint32_t) 0) /* ADC ADCx Regular Data Result Register pos */
#define ADCX_DRST                             ((uint32_t) ((uint32_t)0XFFFF << ADCX_DRST_POS))     /* ADC ADCx(0,1,2) Regular Data Result Register pos filed */


/***************ADCx Data Compare(High) Register  bitfiled ******************/
#define ADCX_COMPH_POS                        ((uint32_t) 0) /* ADC Data Compare(High) Register pos */
#define ADCX_COMPH                            ((uint32_t) ((uint32_t)0XFFFF << ADCX_COMPH_POS))     /* ADC ADCx(0,1,2) Data Compare(High) Register  filed */

#define ADCX_COMPL_POS                        ((uint32_t) 0) /* ADC Data Compare(Low) Register pos */
#define ADCX_COMPL                            ((uint32_t) ((uint32_t)0XFFFF << ADCX_COMPL_POS))     /* ADC ADCx(0,1,2) Data Compare(Low) Register  filed */

/*************** ADC interrupt enable register bitfiled ******************/
#define ADC_JEOS_INTEN_POS                    ((uint32_t) 28)  /* ADC Interrupt enable for injected channel end of sequencepos */
#define ADC_JEOS_INTEN                        ((uint32_t) ((uint32_t)1 << ADC_DFIFO_PREF_INTEN_POS))     /* ADC Interrupt enable for injected channel end of sequence filed */

#define ADC_JEOC_INTEN_POS                    ((uint32_t) 24)  /* ADC Interrupt enable for injected channel end of conversiont enable pos */
#define ADC_JEOC_INTEN                        ((uint32_t) ((uint32_t)1 << ADC_DFIFO_PREF_INTEN_POS))     /* ADC  Interrupt enable for injected channel end of conversion filed */

#define ADC_COMP_MAT_INTEN_POS                ((uint32_t) 20) /* ADC compare match interrupt enable pos */
#define ADC_COMP_MAT_INTEN                    ((uint32_t) ((uint32_t)1 << ADC_COMP_MAT_INTEN_POS))     /* ADC compare match interrupt enable filed */

#define ADC_DFIFO_OVF_INTEN_POS               ((uint32_t) 16) /* ADC data convert overflow interruptenable pos */
#define ADC_DFIFO_OVF_INTEN                   ((uint32_t) ((uint32_t)1 << ADC_DFIFO_OVF_INTEN_POS))    /* ADC data convert overflow interruptenable filed */

#define ADC_DFIFO_PREF_INTEN_POS              ((uint32_t) 12)  /* ADC end of sequence convert interrupt enable pos */
#define ADC_DFIFO_PREF_INTEN                  ((uint32_t) ((uint32_t)1 << ADC_DFIFO_PREF_INTEN_POS))     /* ADC end of sequence convert interrupt enable filed */

#define ADC_DFIFO_FULL_INTEN_POS              ((uint32_t) 8)  /* ADC end of sequence convert interrupt enable pos */
#define ADC_DFIFO_FULL_INTEN                  ((uint32_t) ((uint32_t)1 << ADC_DFIFO_FULL_INTEN_POS))     /* ADC end of sequence convert interrupt enable filed */

#define ADC_EOS_INTEN_POS                     ((uint32_t) 4)  /* ADC end of sequence convert interrupt enable pos */
#define ADC_EOS_INTEN                         ((uint32_t) ((uint32_t)1 << ADC_EOS_INTEN_POS))     /* ADC end of sequence convert interrupt enable filed */

#define ADC_EOC_INTEN_POS                     ((uint32_t) 0)  /* ADC Interrupt enable for regular channel end of conversion pos */
#define ADC_EOC_INTEN                         ((uint32_t) ((uint32_t)1 << ADC_EOC_INTEN_POS))    /* ADC Interrupt enable for regular channel end of conversion enable filed */

/*************** ADC DMA control register bitfiled ***********************/
#define ADC_DMACTR_DFPN_POS                   ((uint32_t) 12) /* ADC channel fifo prefull num pos */
#define ADC_DMACTR_DFPN                       ((uint32_t) ((uint32_t)0x0F << ADC_DMACTR_DFPN_POS)) /* ADC channel fifo prefull num filed */
#define ADC_DMACTR_DFDMAE_POS                 ((uint32_t) 4)  /* Data fifo can transmit data to DMA enable pos */
#define ADC_DMACTR_DFDMAE                     ((uint32_t) ((uint32_t)1 << ADC_DMACTR_DFDMAE_POS))/* Data fifo can transmit data to DMA enable filed */

/*************** ADCX(0,1,2) oversample register bitfiled ************************/
#define ADC_OVSAPL_SFT_POS                    ((uint32_t) 8) /* ADC oversample shift pos */
#define ADC_OVSAPL_SFT                        ((uint32_t) ((uint32_t)0xf << ADC_OVSAPL_SFT_POS)) /* ADC oversample shift filed */
#define ADC_OVSAPL_RATIO_POS                  ((uint32_t) 4) /* ADC oversample ratio pos */
#define ADC_OVSAPL_RATIO                      ((uint32_t) ((uint32_t)7 << ADC_OVSAPL_RATIO_POS)) /* ADC oversample ratio filed */
#define ADC_OVSAPL_EN_POS                     ((uint32_t) 0) /* ADC oversample enable pos */
#define ADC_OVSAPL_EN                         ((uint32_t) ((uint32_t)1 << ADC_OVSAPL_EN_POS)) /* ADC oversample enable filed */

/*************** ADCX(0,1,2) Harware trigger Register Bitfiled *******************/

#define ADC_JHTRIG_SRCSEL_POS                ((uint32_t) 12) /* ADC Hardware trigger enable for injected channels pos */
#define ADC_JHTRIG_SRCSEL                    ((uint32_t) ((uint32_t)0x0F << ADC_JHTRIG_SRCSEL_POS)) /* ADCX(0,1,2) Hardware trigger enable for injected channels filed */

#define ADC_JHTRIG_EN_POS                    ((uint32_t) 8) /* ADC Hardware trigger enable for injected channels pos */
#define ADC_JHTRIG_EN                        ((uint32_t) ((uint32_t)0x1 << ADC_JHTRIG_EN_POS)) /* ADC Hardware trigger enable for injected channels filed */

#define ADC_HWTRIG_SRCSEL_POS                ((uint32_t) 4) /* ADC hardware trigger source select pos */
#define ADC_HWTRIG_SRCSEL                    ((uint32_t) ((uint32_t)0xf << ADC_HWTRIG_SRCSEL_POS)) /* ADC hardware trigger source select filed */
#define ADC_HWTRIG_EN_POS                    ((uint32_t) 0) /* ADC hardware trigger enable pos */
#define ADC_HWTRIG_EN                        ((uint32_t) ((uint32_t)1 << ADC_HWTRIG_EN_POS)) /* ADC hardware trigger enable filed */

/***************ADCx  Injected Channel Selection Register bitfiled ******************/

#define ADCX_JSL_POS                         ((uint32_t) 30) /* ADCX Injected sequence length pos */
#define ADCX_JSL                             ((uint32_t) ((uint32_t)0X3 << ADC_BUSY_ST_POS))     /* ADC ADCx(0,1,2) Injected sequence length filed */

#define ADCX_JCHN3_SEL_POS                   ((uint32_t) 24) /* ADCX Channel select bits of 4th conversion in injected sequence  pos */
#define ADCX_JCHN3_SEL                       ((uint32_t) ((uint32_t)0X1F << ADCX_JCHN0_SEL_POS))     /* ADC Channel select bits of 4th conversion in injected sequence filed */

#define ADCX_JCHN2_SEL_POS                   ((uint32_t) 16) /* ADCX Injected Channel Selection Register pos */
#define ADCX_JCHN2_SEL                       ((uint32_t) ((uint32_t)0X1F << ADCX_JCHN2_SEL_POS))     /* ADC Channel select bits of 3rd conversion in injected sequence filed */

#define ADCX_JCHN1_SEL_POS                   ((uint32_t) 8) /* ADCX Injected Channel Selection Register pos */
#define ADCX_JCHN1_SEL                       ((uint32_t) ((uint32_t)0X1F << ADCX_JCHN1_SEL_POS))     /* ADC Channel select bits of 2nd conversion in injected sequence filed */

#define ADCX_JCHN0_SEL_POS                   ((uint32_t) 0) /* ADCX Injected Channel Selection Register pos */
#define ADCX_JCHN0_SEL                       ((uint32_t) ((uint32_t)0X1F << ADCX_JCHN0_SEL_POS))     /* ADC Channel select bits of 1st conversion in injected sequence filed */

/***************ADCx  Regular Data Result register******************/
#define ADCX_JDRSTY_POS                      ((uint32_t) 0) /*ADCx Injected Data Result Register y pos */
#define ADCX_JDRSTY                          ((uint32_t) ((uint32_t)0XFFFF << ADCX_JDRSTY_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

/***************ADCx(0,1,2) Sample Time Register******************/

#define ADCX_SAMPTM_CH7_POS                  ((uint32_t) 28) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH7                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH6_POS                  ((uint32_t) 24) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH6                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH5_POS                  ((uint32_t) 20) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH5                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH4_POS                  ((uint32_t) 16) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH4                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH3_POS                  ((uint32_t) 12) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH3                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH2_POS                  ((uint32_t) 8) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH2                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH1_POS                  ((uint32_t) 4) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH1                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADCX_SAMPTM_CH0_POS                  ((uint32_t) 0) /*ADCx Injected Data Result Register y pos */
#define ADCX_SAMPTM_CH0                      ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */


/***************ADCx(0,1,2)  Regular Channel Selection Register*****************/
#define ADC_RCHN_SEL5_POS                         ((uint32_t) 25) /*ADCx Channel select bits of 6th conversion in regular sequence  */
#define ADC_RCHN_SEL5                             ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)Channel select bits of 6th conversion in regular sequence  Registerfiled */

#define ADC_RCHN_SEL4_POS                         ((uint32_t) 20) /*ADCx Regular Data Result Register y pos */
#define ADC_RCHN_SEL4                             ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADC_RCHN_SEL3_POS                         ((uint32_t) 15) /*ADCx Regular Data Result Register y pos */
#define ADC_RCHN_SEL3                             ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADC_RCHN_SEL2_POS                         ((uint32_t) 10) /*ADCx Regular Data Result Register y pos */
#define ADC_RCHN_SEL2                             ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADC_RCHN_SEL1_POS                         ((uint32_t) 5) /*ADCx Regular Data Result Register y pos */
#define ADC_RCHN_SEL1                             ((uint32_t) ((uint32_t)0X0F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADC_RCHN_SEL0_POS                         ((uint32_t) 0) /*ADCx Regular Data Result Register y pos */
#define ADC_RCHN_SEL0                             ((uint32_t) ((uint32_t)0X1F << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)ADCx Injected Data Result Registerfiled */

#define ADC_RCHN2_RSL_POS                         ((uint32_t) 28) /*ADCx Regular channel sequence length */
#define ADC_RCHN2_RSL                             ((uint32_t) ((uint32_t)0XF << ADCX_SAMPTM_CH0_POS))     /* ADC ADCx(0,1,2)Regular channel sequence length Registerfiled */


/***************ADC External Channel Enable Register*****************/
#define ADC_EXCH20_EN_POS                         ((uint32_t) 0) /*ADCx ADC external channel N enable */
#define ADC_EXCH20_EN                             ((uint32_t) ((uint32_t)0X1 << ADC_EXCH20_EN_POS))     /* ADCexternal channel N enable filed */

#define ADC_EXCH0_EN_POS                          ((uint32_t) 0) /*ADCx ADC external channel N enable */
#define ADC_EXCH0_EN                              ((uint32_t) ((uint32_t)0X1 << ADC_EXCH0_EN_POS))     /* ADCexternal channel N enable filed */

/***************ADC Multi Mode Controller Register*****************/
#define ADC_CPEN_POS                              ((uint32_t) 28) /*ADC charge pump enable for three analog converters */
#define ADC_CPEN_EN                               ((uint32_t) ((uint32_t)0X1 << ADC_CPEN_POS))     /* ADC charge pump enable for three analog converters filed */

#define ADC_INTER_DELAY_POS                       ((uint32_t) 8) /*ADCx Delay between two conversions in interleaved mode*/
#define ADC_INTER_DELAY                           ((uint32_t) ((uint32_t)0xF << ADC_INTER_DELAY_POS))     /* ADC Delay between two conversions in interleaved mode filed */


#define ADC_MULTI_NUM_POS                         ((uint32_t) 4) /*ADCx Dual or triple ADC mode selection bit */
#define ADC_MULTI_NUM                             ((uint32_t) ((uint32_t)0x1 << ADC_MULTI_NUM_POS))     /* ADC Dual or triple ADC mode selection bit */

#define ADC_MULTI_MODE_POS                        ((uint32_t) 0) /*ADCx ADC multi mode selection */
#define ADC_MULTI_MODE                            ((uint32_t) ((uint32_t)0X7 << ADC_EXCH0_EN_POS))     /* ADC multi mode selection */

/**
 * @brief Enable ADC interrupt.
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define ADC_INTERRUPT_ENABLE(pADC, mask)                       IO_BIT_SET(pADC->IE, mask)

/**
 * @brief Disable ADC interrupt.
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define ADC_INTERRUPT_DISABLE(pADC, mask)                      IO_BIT_CLEAR(pADC->IE, mask)


/**
 * @brief Config ADC convert mode singal or repart.
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert mode
 *
 * @retval    None
 */
#define CONFIG_CONTINOUS_MODE(pADC, value)                     IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_WM, value << ADC_CTRL_WM_POS)

/**
 * @brief Disable ADC
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_DISABLE(pADC)                                      IO_BIT_CLEAR(pADC->CTRL, ADC_CTRL_EN)

/**
 * @brief Enable ADC
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_ENABLE(pADC)                                       IO_BIT_SET(pADC->CTRL, ADC_CTRL_EN)

/**
 * @brief config ADC convert resolution
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert resolution
 *
 * @retval    None
 */
 #define ADC_CONFIG_RESOLUTION(pADC, value)                     IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_BNIT, value << ADC_CTRL_BNIT_POS)


/**
 * @brief Triger ADC to start convert
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_SW_TRIGGER(pADC)                                   IO_BIT_SET(pADC->TRIG, ADC_TRIG_CT)


/**
 * @brief Enable ADC convert result match function
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_COMPARE_ENABLE(pADC)                               IO_BIT_SET(pADC->CTRL, ADC_CTRL_CE)

/**
 * @brief Disable ADC convert result match function
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_COMPARE_DISABLE(pADC)                              IO_BIT_CLEAR(pADC->CTRL, ADC_CTRL_CE)


/**
 * @brief Enable ADC convert result match function
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_COMPARE_SINGLE_ENABLE(pADC)							IO_BIT_SET(pADC->CTRL, ADC_COMP_SING_EN)

/**
 * @brief Disable ADC convert result match function
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_COMPARE_SINGLE_DISABLE(pADC)						IO_BIT_CLEAR(pADC->CTRL, ADC_COMP_SING_EN)

/**
 * @brief Set ADC convert data fifo prefull num
 *
 * @param     pADC ADC devive point
 * @param     value ADC prefull num
 *
 * @retval    None
 */
#define ADC_DFIFO_PREFULL_NUM(pADC, value)                      IO_BITMASK_SET(pADC->DMACTR, ADC_DMACTR_DFPN, value << ADC_DMACTR_DFPN_POS)

/**
 * @brief Set ADC Auto Compare Channel Selection Register
 *
 * @param     pADC ADC devive point
 * @param     Auto Compare channel num index
 *
 * @retval    None
 */
#define ADC_SET_COMP_CHANNEL_INDEX(pADC, channelIndex)          (pADC->COM_CHN = channelIndex)

/**
 * @brief clear ADC interrupt accordint to bitmask
 *
 * @param     pADC ADC devive point
 * @param     Mask interrupt mask bit
 *
 * @retval    None
 */
#define ADC_CLEAR_INTERRUPT(pADC, Mask)                        (pADC->ST = (Mask))

/**
 * @brief clear ADC interrupt, overflow, eos interrupt.
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_CLEAR_ALL_INTERRUPT(pADC)                          (pADC->ST = pADC->STATUS)

/**
 * @brief clear ADC interrupt, overflow, eos interrupt.
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define ADC_SET_COMPARE_MODE(pADC, value)                      IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_CM, value << ADC_CTRL_CM_POS)

/**
 * @brief Config ADC convert wheather to use DMA or not
 *
 * @param     pADC ADC devive point
 * @param     value enable or disable DMA to transfer
 *
 * @retval    None
 */
#define ADC_CONFIG_DMA_ENABLE(pADC, value)                      do{ \
                                                                    IO_BITMASK_SET(pADC->DMACTR, ADC_DMACTR_DFDMAE, \
                                                                                   value << ADC_DMACTR_DFDMAE_POS); \
																	} while(0)

/**
 * @brief Set ADC convert match value
 *
 * @param      pADC ADC  devive point
 * @param      lowvalue  low convert match value
 * @param      highvalue high convert match value
 *
 * @retval     None
 */
#define ADC_SET_COMPARE_VALUE(pADC, lowValue, highValue)        do{                                                         \
                                                                    pADC->COMPL = lowValue;                                 \
                                                                    pADC->COMPH = highValue;                                \
                                                                }while(0)
/**
 * @brief Resert ADC ip
 * @retval    None
 */
#define ADC_RESET()
/**
 * @brief Enable ADC clock
 * @retval None
 */
#define ADC_CLOCK_ENABLE()                                      do{                                                   \
                                                                    Clock_PeripheralClockEnable(ADC_CLOCK_GATE_MASK); \
                                                                }while(0)    /* Enable ADC ip clk */

/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
/**
 * @}
 */

/**
 * @defgroup TIMEOUT_PARAMETER
 * Specify general timeout parameter of blocking and non-blocking function
 * @{
 */
#define TIMEOUT_NO_WAIT      0x00000000 /**< Transaction is over after function returns, it returns RET_ERROR if transaction is not finished. */
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF /**< Function is about to block till transaction is over */
#define TIMEOUT_NO_BLOCKING  0xFFFFFFFE /**< Function is never blocked, user will be notified by callback after transaction is over */
/**
 * @}
 */


/******************************* ADC Instances ********************************/
#define IS_ADC_ALL_INSTANCE(INSTANCE) (((INSTANCE) == ADC1) || \
                                       ((INSTANCE) == ADC2) || \
                                       ((INSTANCE) == ADC3))

#define IS_ADC_RESOLUTION(RESOLUTION) (((RESOLUTION) == ADC_RESOLUTION_12BIT) || \
                                       ((RESOLUTION) == ADC_RESOLUTION_10BIT) || \
                                       ((RESOLUTION) == ADC_RESOLUTION_8BIT)

/**
  * @brief  Select  ADC work mode mode.
  * @param  01: Single mode; 11: Repeat mode
  * @retval None
  */
#define ADC_CTRL_WORKMODE(_WORK_MODE_) ((_WORK_MODE_) << 16)

#define ADC_CTRL_RESOLUTION(_RESOLUTION_) ((_RESOLUTION_) << ADC_CTRL_BNIT_POS)

#define ADC_CTRL_LOWPOWER(EN) ((EN) << ADC_CTRL_LPEN_POS)


/**
  * @brief  Enable ADC continuous conversion mode.
  * @param  _CONTINUOUS_MODE_: Continuous mode.
  * @retval None
  */
#define ADC_CTRL_DISCONTINUOUS(_DISCONTINUOUS_MODE_) ((_DISCONTINUOUS_MODE_) << 12)


/**
  * @brief  Enable the ADC peripheral.
  * @param  __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CTRL |=  ADC_CTRL_EN)

/**
  * @brief  Disable the ADC peripheral.
  * @param  __HANDLE__: ADC handle
  * @retval None
  */
#define __HAL_ADC_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CTRL &=  ~ADC_CTRL_EN)

/**
  * @brief  Enable the ADC end of conversion interrupt.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __INTERRUPT__: ADC Interrupt.
  * @retval None
  */
#define __HAL_ADC_ENABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->IE) |= (__INTERRUPT__))

/**
  * @brief  Disable the ADC end of conversion interrupt.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __INTERRUPT__: ADC interrupt.
  * @retval None
  */
#define __HAL_ADC_DISABLE_IT(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->IE) &= ~(__INTERRUPT__))

/** @brief  Check if the specified ADC interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __INTERRUPT__: specifies the ADC interrupt source to check.
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_ADC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)  (((__HANDLE__)->Instance->IE & (__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Clear the ADC's pending flags.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __FLAG__: ADC flag.
  * @retval None
  */
#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->ST) = (__FLAG__))

/**
  * @brief  Get the selected ADC's flag status.
  * @param  __HANDLE__: specifies the ADC Handle.
  * @param  __FLAG__: ADC flag.
  * @retval None
  */
#define __HAL_ADC_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->ST) & (__FLAG__)) == (__FLAG__))


/**
  * @brief  Set ADC Regular channel sequence length.
  * @param  _NbrOfConversion_: Regular channel sequence length.
  * @retval None
  */
#define ADC_RCHN_L(_NbrOfConversion_) (((_NbrOfConversion_) - (uint8_t)1) << 28)


/**
  * @brief  Set the ADC's sample time for channel numbers between 10 and 18.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SMPR1(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * (((uint32_t)((uint16_t)(_CHANNELNB_))) - 10)))

/**
  * @brief  Set the ADC's sample time for channel numbers between 0 and 7.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SAMPTM0(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (4 * ((uint32_t)((uint16_t)(_CHANNELNB_)))))

/**
  * @brief  Set the ADC's sample time for channel numbers between 8 and 15.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SAMPTM1(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (4 * (((uint32_t)((uint16_t)(_CHANNELNB_))) -8)))

/**
  * @brief  Set the ADC's sample time for channel numbers between 16 and 23.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SAMPTM2(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (4 * (((uint32_t)((uint16_t)(_CHANNELNB_))) -16)))

/**
  * @brief  Set the ADC's sample time for channel numbers between 24 and 31.
  * @param  _SAMPLETIME_: Sample time parameter.
  * @param  _CHANNELNB_: Channel number.
  * @retval None
  */
#define ADC_SAMPTM3(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (4 * (((uint32_t)((uint16_t)(_CHANNELNB_))) -24)))

/**
  * @brief  Set the selected regular channel rank for rank between 0 and 5.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @retval None
  */
#define ADC_SQR0_RK(_CHANNELNB_, _SHFIT_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * ((_SHFIT_))))

/**
  * @brief  Set the selected regular channel rank for rank between 6 and 11.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @retval None
  */
#define ADC_SQR1_RK(_CHANNELNB_, _SHFIT_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * ((_SHFIT_) - 6)))

/**
  * @brief  Set the selected regular channel rank for rank between 12 and 15.
  * @param  _CHANNELNB_: Channel number.
  * @param  _RANKNB_: Rank number.
  * @retval None
  */
#define ADC_SQR2_RK(_CHANNELNB_, _SHFIT_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5 * ((_SHFIT_) - 12)))

#define ADC_AUTOCOMP_CHN_SELECT(_CHANNELNB_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << 16)


#define ADC_JSLEN(_SEQLEN_) (((uint32_t)((uint16_t)(_SEQLEN_))) << 30)



#define IS_ADC_CHANNEL(CHANNEL) ((CHANNEL) <= 31)

#define IS_ADC_REGULAR_SEQ_MAX_LEN(_SEQ_LEN_) (((_SEQ_LEN_) <= 16) || ((_SEQ_LEN_) > 0))


#define IS_ADC_INJECTED_LENGTH(_INJECTNBROFCOV_) ((_INJECTNBROFCOV_) <= 3)

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#define IS_ADC_SAMPLE_TIME(TIME) (((TIME) == ADC_SAMPLETIME_3CYCLES)   || \
                                  ((TIME) == ADC_SAMPLETIME_4CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_6CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_8CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_12CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_16CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_24CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_32CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_48CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_64CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_96CYCLES)  || \
                                  ((TIME) == ADC_SAMPLETIME_128CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_192CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_256CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_384CYCLES) || \
                                  ((TIME) == ADC_SAMPLETIME_512CYCLES))


typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

#define HAL_IS_BIT_SET(REG, BIT)         (((REG) & (BIT)) != RESET)
#define HAL_IS_BIT_CLR(REG, BIT)         (((REG) & (BIT)) == RESET)

/** @defgroup ADCEx_injected_channel_selection ADC Injected Channel Selection
  * @{
  */
#define ADC_INJECTED_RANK_0    ((uint32_t)0x00000000)
#define ADC_INJECTED_RANK_1    ((uint32_t)0x00000001)
#define ADC_INJECTED_RANK_2    ((uint32_t)0x00000002)
#define ADC_INJECTED_RANK_3    ((uint32_t)0x00000003)

 #define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0U)

#define __HAL_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    }while (0U)

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1)
	{
		printk("assert_param error\n");
	}
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ADC_DEV_H_ */

