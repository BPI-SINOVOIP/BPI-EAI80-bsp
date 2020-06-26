
ugelis_include_directories(.)
ugelis_sources_ifdef(CONFIG_RTT_CONSOLE rtt/SEGGER_RTT.c)
ugelis_sources_ifdef(CONFIG_SEGGER_SYSTEMVIEW systemview/SEGGER_SYSVIEW.c)
