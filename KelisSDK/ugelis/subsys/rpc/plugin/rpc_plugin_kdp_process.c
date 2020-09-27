#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <gm_hal_rpc.h>
#include <rpc_core.h>
#include <rpc_plugin_yolo.h>
#include <gm_hal_kdp.h>
#include <ops.h>

typedef struct RPC_input_private
{
	unsigned char *camera_input;
	unsigned short *input;
	unsigned short int *output;
	unsigned char *weight;
} RPC_input_private_T;

static RET_CODE predict(struct easynet_dev *dev, unsigned short *input, unsigned short *output, unsigned char *weight)
{
    RET_CODE ret = RET_ERROR;

	HAL_CNN_Bram_hfwd_rw_en(ENABLE);
	
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
        	if(ret == 0x06)
        	{
				dev = easynet_process_update(weight, input, output);
			}
            break;
        }
    } while (1);


	HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);

	if(ret == 0x01)
		return RET_ERROR;

	return RET_OK;
}

static RET_CODE RPC_PrePocess(RPC_Package_T *pack)
{
	RET_CODE ret = RET_ERROR;
	struct easynet_dev *dev;
	RPC_input_private_T *prpc_pri =  (RPC_input_private_T *)pack->privData;

	dev = easynet_process_init(prpc_pri->weight, prpc_pri->input, prpc_pri->output);
	dev->input = prpc_pri->camera_input;

	//printf("slave dev = 0x%x,input=0x%x, output=0x%x, weight=0x%x \n",  prpc_pri->camera_input, prpc_pri->input, prpc_pri->output,prpc_pri->weight);
	ret = predict(dev,prpc_pri->input,prpc_pri->output,prpc_pri->weight);
	return RET_OK;

	 
}

static int RPC_YoloInit (struct device *dev)
{
    RET_CODE ret = RET_OK;
	ret = RPC_SlaveRegisterCmd(RPC_PRE_PROCESSING, RPC_PrePocess);
	return ret;
}


DEVICE_DEFINE(rpc_yolo, DEV_RPC_YOLO_NAME, RPC_YoloInit, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;



