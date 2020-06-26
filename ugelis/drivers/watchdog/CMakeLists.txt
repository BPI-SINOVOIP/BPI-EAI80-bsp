ugelis_sources_ifdef(CONFIG_WDT_QMSI wdt_qmsi.c)
ugelis_sources_ifdef(CONFIG_IWDG_STM32 iwdg_stm32.c)
ugelis_sources_ifdef(CONFIG_WDOG_CMSDK_APB wdog_cmsdk_apb.c)
ugelis_sources_ifdef(CONFIG_WDT_SAM wdt_sam.c)
ugelis_sources_ifdef(CONFIG_WDT_ESP32 wdt_esp32.c)
ugelis_sources_ifdef(CONFIG_WDT_SAM0 wdt_sam0.c)
ugelis_sources_ifdef(CONFIG_WDT_NRFX wdt_nrfx.c)
ugelis_sources_ifdef(CONFIG_WDT_MCUX_WDOG wdt_mcux_wdog.c)
ugelis_sources_ifdef(CONFIG_IWDG_GM iwdg_gm.c)
ugelis_sources_ifdef(CONFIG_WWDG_GM wwdg_gm.c)


ugelis_library_sources_ifdef(CONFIG_USERSPACE   watchdog_handlers.c)
