# Board initialization
ugelis_sources_ifdef(CONFIG_PINMUX_CC2650          pinmux_cc2650.c)
ugelis_sources_ifdef(CONFIG_PINMUX_ESP32           pinmux_esp32.c)
ugelis_sources_ifdef(CONFIG_PINMUX_FE310           pinmux_fe310.c)
ugelis_sources_ifdef(CONFIG_PINMUX_MCUX            pinmux_mcux.c)
ugelis_sources_ifdef(CONFIG_PINMUX_MCUX_LPC        pinmux_mcux_lpc.c)
ugelis_sources_ifdef(CONFIG_PINMUX_QMSI            pinmux_qmsi.c)
ugelis_sources_ifdef(CONFIG_PINMUX_STM32           stm32/pinmux_stm32.c)
ugelis_sources_ifdef(CONFIG_PINMUX_SAM0            pinmux_sam0.c)
ugelis_sources_ifdef(CONFIG_PINMUX_GM              gm/pinmux_gm.c)

# "runtime" pinmux
add_subdirectory_ifdef(CONFIG_PINMUX_DEV dev)
