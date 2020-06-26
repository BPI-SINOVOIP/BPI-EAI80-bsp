# Put functions and data in their own binary sections so that ld can
# garbage collect them
ugelis_cc_option(-ffunction-sections -fdata-sections)

ugelis_sources_ifdef(
  CONFIG_GEN_ISR_TABLES
  isr_tables.c
  )

ugelis_sources_ifdef(
  CONFIG_EXECUTION_BENCHMARKING
  timing_info_bench.c
  )
