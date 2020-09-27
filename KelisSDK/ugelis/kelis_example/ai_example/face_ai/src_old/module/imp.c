#include <display/fb_display.h>

#include <gm_hal_imp.h>
#include <fmath.h>

#define IMP_DEVICE (IMP_Device_T *)(0x40024000)

void ai_init_imp()
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    uint32_t input_offset = 0;
    uint32_t output_offset = 0;
    uint32_t input_pixfmt  = IMP_INPUT_RGB888;
    uint32_t output_pixfmt = FMT_RGB888;

    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
    layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);
}

int ai_imp_scaleup(uint32_t src, uint32_t dst, uint32_t src_width, uint32_t src_h, uint32_t dst_w, uint32_t dst_h)
{

    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t input_offset = 0;
    uint32_t output_offset = 0;
    uint32_t timeout = 0;

    float w_scale = 0.0;
    float h_scale = 0.0;

    pixel_num = src_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

    w_scale = (float)pixel_num / (dst_w - output_offset);
    h_scale = (float)src_h / dst_h;

    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = src_h;
    scaleCfg.scaleWidth = (dst_w - output_offset);
    scaleCfg.scaleHeight = dst_h;
    scaleCfg.svi = fast_floorf(h_scale);
    scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi) * 256);
    scaleCfg.shi = fast_floorf(w_scale);
    scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi) * 256);

    HAL_IMP_Start(pIMP, (uint32_t)src, (uint32_t)dst, &scaleCfg);

    while (__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break;
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);

    return 0;
}

