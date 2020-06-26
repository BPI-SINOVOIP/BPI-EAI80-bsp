ugelis_sources(
  soc.c
  power.c
  )
ugelis_sources_ifdef(CONFIG_ARM_MPU_ENABLE arm_mpu_regions.c)
