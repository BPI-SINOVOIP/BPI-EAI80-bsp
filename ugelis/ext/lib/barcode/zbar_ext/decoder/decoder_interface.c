
#ifndef __DECODER_INTERFACE_C
#define __DECODER_INTERFACE_C
#ifdef __cplusplus
extern "C" {
#endif

#include <config_zbar.h>
	
#if defined(ENABLE_CODE128)
#include "code128.c"
#endif

#if defined(ENABLE_CODE39)
#include "code39.c"
#endif

#if defined(ENABLE_EAN)
#include "ean.c"
#endif

#if defined(ENABLE_I25)
#include "i25.c"
#endif

#if defined(ENABLE_PDF417)
#include "pdf417.c"
#endif

#if defined(ENABLE_QRCODE)
#include "qr_finder.c"
#endif


#ifdef __cplusplus
}
#endif
#endif /* __SRC_CAMERA_INTERFACE_COMMON_C */
