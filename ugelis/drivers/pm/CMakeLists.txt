enable_language(C ASM)
ugelis_sources_ifdef(CONFIG_PM	        pm_core.c) 
ugelis_sources_ifdef(CONFIG_PM          power_gcc_asm/power_core_asm.s)
ugelis_sources_ifdef(CONFIG_POWER_GM	power_gm.c)
ugelis_sources_ifdef(CONFIG_POWER_GM    power_gcc_asm/power_gm_asm.s)
ugelis_sources_ifdef(CONFIG_USERSPACE   power_handlers.c)






