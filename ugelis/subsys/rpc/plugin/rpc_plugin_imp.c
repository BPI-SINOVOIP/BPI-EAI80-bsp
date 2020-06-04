#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <gm_hal_rpc.h>
#include <rpc_core.h>
#include <rpc_plugin_imp.h>
#include <imlib.h>
//#include <sobel.h>

//#define SLAVE_IMP_DEBUG 1

#ifdef SLAVE_IMP_DEBUG
    #define SlaveImpDebug       printk
#else
    #define SlaveImpDebug(...)  do{}while(0);
#endif

static int32_t time_old = 0;
static int32_t time_new = 0;
static int32_t time_diff = 0;

static RET_CODE RPC_ImpPocess(RPC_Package_T *pack)
{
    RET_CODE ret = RET_ERROR;
    IMP_Data_T *impData = (IMP_Data_T *)pack->privData;

    SlaveTrace("[%s(%d)] impCmd=%d\n", __FUNCTION__, __LINE__, impData->impCmd);
    switch(impData->impCmd)
    {
        case IMP_CMD_SOBEL:
            SlaveImpDebug("IMP_CMD_SOBEL w:%d, h:%d\n", impData->width, impData->height);
            //SlaveImpDebug("0xE0042024=0x%x\n", *(uint32_t*)0xE0042024);
            time_old = k_uptime_get_32();
            imlib_sobel(impData->pSrc, impData->height, impData->width, impData->pDst);
            time_new = k_uptime_get_32();
            time_diff = time_new - time_old;
            SlaveImpDebug("[slave sobel cost]: %d ms\n", time_diff);
            ret = RET_OK;
            break;
        case IMP_CMD_FILTER:
            SlaveImpDebug("IMP_CMD_FILTER w:%d, h:%d\n", impData->width, impData->height);
            SlaveImpDebug("0xE0042024=0x%x\n", *(uint32_t*)0xE0042024);
            time_old = k_uptime_get_32();
            /* median filter */
            image_t img;
            img.bpp = IMAGE_BPP_GRAYSCALE;//IMAGE_BPP_RGB565;
            img.w = impData->width; img.h = impData->height;
            img.data = impData->pSrc;//image_addr;
            int ksize = 1;
            imlib_median_filter(&img, ksize, 12, false, 0, false, NULL);
            if(impData->pSrc != impData->pDst)
            {
                SlaveImpDebug("slave filter memcpy\n");
                memcpy(impData->pDst, impData->pSrc, impData->width*impData->height);
            }
            time_new = k_uptime_get_32();
            time_diff = time_new - time_old;
            SlaveImpDebug("[slave filter cost]: %d ms\n", time_diff);
            ret = RET_OK;
            break;
        case IMP_CMD_BINARY:
            SlaveImpDebug("IMP_CMD_BINARY ToDo...\n");
            ret = RET_OK;
            break;
        default:
            SlaveImpDebug("imp_cmd not support\n");
            ret = RET_ERROR;
            break;
    }

    SlaveTrace("[%s(%d)] done\n", __FUNCTION__, __LINE__);
    return ret;
}

static int RPC_ImpInit (struct device *dev)
{
    RET_CODE ret = RET_OK;
	ret = RPC_SlaveRegisterCmd(RPC_CMD_IMP, RPC_ImpPocess);
	return ret;
}


DEVICE_DEFINE(rpc_imp, DEV_RPC_IMP_NAME, RPC_ImpInit, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;



