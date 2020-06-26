ugelis_library()

ugelis_library_sources_ifdef(CONFIG_I2S_SAM_SSC		i2s_sam_ssc.c)
ugelis_library_sources_ifdef(CONFIG_I2S_CAVS		i2s_cavs.c)
ugelis_library_sources_ifdef(CONFIG_SAI_GM		    i2s_gm.c)