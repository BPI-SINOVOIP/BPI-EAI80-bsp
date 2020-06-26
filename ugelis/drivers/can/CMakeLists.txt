ugelis_sources_ifdef(CONFIG_CAN_STM32	stm32_can.c)
ugelis_sources_ifdef(CONFIG_CAN_GM      gm_can.c)
ugelis_sources_ifdef(CONFIG_USERSPACE   can_handlers.c)
