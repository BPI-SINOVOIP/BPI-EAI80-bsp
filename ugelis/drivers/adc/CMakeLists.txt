ugelis_library()

ugelis_library_sources_ifdef(CONFIG_ADC_DW            adc_dw.c)
ugelis_library_sources_ifdef(CONFIG_ADC_MCUX_ADC16 	  adc_mcux_adc16.c)
ugelis_library_sources_ifdef(CONFIG_ADC_QMSI   		  adc_qmsi.c)
ugelis_library_sources_ifdef(CONFIG_ADC_QMSI_SS		  adc_qmsi_ss.c)
ugelis_library_sources_ifdef(CONFIG_ADC_SAM_AFEC      adc_sam_afec.c)
ugelis_library_sources_ifdef(CONFIG_ADC_TI_ADC108S102 adc_ti_adc108s102.c)
ugelis_library_sources_ifdef(CONFIG_ADC_GM            adc_gm.c)

ugelis_library_sources_ifdef(CONFIG_USERSPACE   adc_handlers.c)
