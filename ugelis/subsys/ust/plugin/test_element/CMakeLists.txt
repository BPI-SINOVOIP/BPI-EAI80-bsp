ugelis_include_directories(
    ${UGELIS_BASE}/include/ust/plugin/test
    ${UGELIS_BASE}/include/ust/core
)

ugelis_sources_ifdef(CONFIG_SRC_ELEMENT  src_element.c)
ugelis_sources_ifdef(CONFIG_SINK_ELEMENT sink_element.c)
ugelis_sources_ifdef(CONFIG_MID_ELEMENT  mid_element.c)