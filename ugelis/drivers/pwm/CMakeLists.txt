ugelis_library()

ugelis_library_sources_ifdef(CONFIG_PWM_PCA9685 	pwm_pca9685.c)
ugelis_library_sources_ifdef(CONFIG_PWM_DW		pwm_dw.c)
ugelis_library_sources_ifdef(CONFIG_PWM_QMSI		pwm_qmsi.c)
ugelis_library_sources_ifdef(CONFIG_PWM_STM32		pwm_stm32.c)
ugelis_library_sources_ifdef(CONFIG_PWM_NRF5_SW		pwm_nrf5_sw.c)
ugelis_library_sources_ifdef(CONFIG_PWM_NRFX		pwm_nrfx.c)
ugelis_library_sources_ifdef(CONFIG_PWM_MCUX_FTM 	pwm_mcux_ftm.c)
ugelis_library_sources_ifdef(CONFIG_PWM_IMX		pwm_imx.c)
ugelis_library_sources_ifdef(CONFIG_PWM_LED_ESP32	pwm_led_esp32.c)

ugelis_library_sources_ifdef(CONFIG_USERSPACE   pwm_handlers.c)
