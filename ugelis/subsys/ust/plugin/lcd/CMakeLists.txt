ugelis_include_directories(
    ${UGELIS_BASE}/include/ust/plugin
    ${UGELIS_BASE}/include/ust/core
)

ugelis_sources_ifdef(CONFIG_UST_PLUGIN_LCD  ust_lcd.c)