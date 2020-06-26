ugelis_library()

ugelis_library_sources_ifdef(CONFIG_RTC_QMSI rtc_qmsi.c)
ugelis_library_sources_ifdef(CONFIG_RTC_MCUX rtc_mcux.c)
ugelis_library_sources_ifdef(CONFIG_RTC_GM   rtc_gm.c)
ugelis_library_sources_ifdef(CONFIG_USERSPACE   rtc_handlers.c)
