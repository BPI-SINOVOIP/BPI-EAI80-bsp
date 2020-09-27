#ifndef _CRYPTO_TEST_H_
#define _CRYPTO_TEST_H_

/** AES ECB mode test */
int CRYP_AesEcbTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** AES CBC mode test */
int CRYP_AesCbcTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** AES CTR mode test */
int CRYP_AesCTRTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** AES GCM mode test */
int CRYP_AesGCMTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** AES CCM mode test */
int CRYP_AesCCMTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** DES/TDES ECB mode test */
int CRYP_DESEcbTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** DES/TDES CBC mode test */
int CRYP_DESCbcTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

/** AES ECB swap test */
int CRYP_AesEcbSwapTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config);

#endif






