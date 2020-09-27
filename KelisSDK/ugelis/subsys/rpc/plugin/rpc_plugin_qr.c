#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <gm_hal_rpc.h>
#include <rpc_core.h>
#include <rpc_plugin_qr.h>

#include <config_zbar.h>
//#include <unistd.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#include <stdlib.h>     /* malloc, free */
//#include <time.h>       /* clock_gettime */
//#include <sys/time.h>   /* gettimeofday */
#include <string.h>     /* memcmp, memset, memcpy */
#include <assert.h>

#include <zbar.h>
#include "error.h"
#include "image.h"
#ifdef ENABLE_QRCODE
# include "qrcode.h"
#endif
#include "img_scanner.h"
#include "svg.h"

#ifdef CONFIG_QR_DUAL_EN
#include <rpc_core.h>
#include <rpc_plugin_qr.h>
#endif



//#define SLAVE_QR_DEBUG 1

#ifdef SLAVE_QR_DEBUG
    #define SlaveQrDebug       printk
#else
    #define SlaveQrDebug(...)  do{}while(0);
#endif

static int32_t time_old = 0;
static int32_t time_new = 0;
static int32_t time_diff = 0;


static RET_CODE RPC_QrPocess(RPC_Package_T *pack)
{
    RET_CODE ret = RET_ERROR;
    QR_Data_T *qrData = (QR_Data_T *)pack->privData;
    zbar_image_scanner_t *iscn = qrData->iscn;
    zbar_image_t *img = qrData->img;

    SlaveTrace("[%s(%d)] iscn=0x%x, img=0x%x\n", __FUNCTION__, __LINE__, iscn, img);
    zbar_scan_image_x(iscn, img);

    SlaveTrace("[%s(%d)] done\n", __FUNCTION__, __LINE__);
    return RET_OK;
}

static int RPC_QrInit (struct device *dev)
{
    RET_CODE ret = RET_OK;
	ret = RPC_SlaveRegisterCmd(RPC_CMD_QR, RPC_QrPocess);
	return ret;
}


DEVICE_DEFINE(rpc_qr, DEV_RPC_QR_NAME, RPC_QrInit, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;



