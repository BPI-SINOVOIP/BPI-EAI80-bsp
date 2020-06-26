
if(CONFIG_GM_HAL_BUILTIN)
##############################-I############################
ugelis_include_directories(

  ${UGELIS_BASE}/ext/hal/cmsis/Include
  ${UGELIS_BASE}/ext/hal/gree
  ${UGELIS_BASE}/ext/hal/gree
  ${UGELIS_BASE}/ext/hal/gree/driver
  ${UGELIS_BASE}/ext/hal/gree/include
  ${UGELIS_BASE}/ext/hal/gree/include/lib/common
  ${UGELIS_BASE}/ext/hal/gree/include/config
  ${UGELIS_BASE}/ext/hal/gree/include/driver
  ${UGELIS_BASE}/ext/hal/gree/include/arch/arm/soc
  ${UGELIS_BASE}/ext/hal/gree/include/arch/arm/cpu
)

##############################compiler for common############################
    ugelis_sources(lib/common/gm_common.c)
##############################compiler for soc##############################
    ugelis_sources_ifdef(CONFIG_SOC_SERIES_GM6721X arch/arm/soc/gm6721x/soc_gm6721x.c)
##############################complier driver#################################
    ugelis_sources_ifdef(CONFIG_GM_ARM_HAL_MPU arch/arm/cpu/mpu/arm_hal_mpu.c)
    ugelis_sources_ifdef(CONFIG_GM_ARM_HAL_FLASH_PATCH arch/arm/cpu/flash_patch/arm_hal_flash_patch.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_CACHE arch/arm/cpu/cache/gm_hal_cache.c)
    ugelis_sources_ifdef(CONFIG_GM_ARM_HAL_BUS_MAPPING arch/arm/cpu/bus_map/gm_hal_bus_map.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_RPC driver/rpc/gm_hal_rpc.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_UART driver/uart/gm_hal_uart.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_CAN driver/can/gm_hal_can.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_VIN driver/vin/gm_hal_vin.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_VOUT driver/vout/gm_hal_vout.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_USB driver/usb/gm_hal_usb.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_GPIO driver/gpio/gm_hal_gpio.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_DMA driver/dma/gm_hal_dma.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_ADC driver/adc/gm_hal_adc.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_IMP driver/imp/gm_hal_imp.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_PCD driver/usb/gm_hal_pcd.c driver/usb/gm_ll_usb.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_IWDG driver/iwdg/gm_hal_iwdg.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_WWDG driver/wwdg/gm_hal_wwdg.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_TRNG driver/trng/gm_hal_trng.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_CRYPTO driver/crypto/gm_hal_crypto.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_HASH driver/hash/gm_hal_hash.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_FMC driver/fmc/gm_hal_fmc.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_OTP driver/otp/gm_hal_otp.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_PES driver/pes/gm_hal_pes.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_SAI driver/sai/gm_hal_sai.c)
	ugelis_sources_ifdef(CONFIG_GM_HAL_PDM driver/pdm/gm_hal_pdm.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_HCD driver/usb/gm_hal_hcd.c driver/usb/gm_ll_usb.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_SQI driver/sqi/gm_hal_sqi.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_SQI driver/sqi/gm_hal_sqi_host.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_SPI driver/spi/gm_hal_spi.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_MMC driver/mmc/block/hal_mmc.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_MMC driver/mmc/core/hal_mmc_core.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_MMC driver/mmc/host/gm_hal_mmc_host.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_RTC driver/rtc/gm_hal_rtc.c driver/rtc/gm_hal_utc.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_TIM driver/tim/gm_hal_tim.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_SDRAM driver/sdram/gm_hal_sdram.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_NF driver/nandflash/gm_hal_nf.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_ACMP driver/acmp/gm_hal_acmp.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_CLOCK driver/clock/gm_hal_clock.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_DAC driver/dac/gm_hal_dac.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_LVD driver/lvd/gm_hal_lvd.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_RESET driver/reset/gm_hal_reset.c)
    ugelis_sources_ifdef(CONFIG_GM_HAL_POWER driver/power/gm_hal_power.c)
	ugelis_sources_ifdef(CONFIG_GM_HAL_I2C driver/i2c/gm_hal_i2c.c)
	
	ugelis_sources(driver/kdp/gm_hal_kdp.c)
	ugelis_sources(driver/sqi/gm_hal_sqi.c)
	ugelis_sources(driver/sqi/gm_hal_sqi_host.c)
##Add Other Hal Driver Here###
##Add Other Hal Driver Here###
endif()





