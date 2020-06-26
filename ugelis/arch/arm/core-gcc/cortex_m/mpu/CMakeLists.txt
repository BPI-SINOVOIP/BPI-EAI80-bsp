ugelis_library()

ugelis_library_sources_ifdef(CONFIG_ARM_CORE_MPU arm_core_mpu.c)
ugelis_library_sources_if_kconfig(               arm_mpu.c)
ugelis_library_sources_if_kconfig(               nxp_mpu.c)
