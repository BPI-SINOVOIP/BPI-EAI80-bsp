ugelis_include_directories(
    ${UGELIS_BASE}/include/rpc/plugin
)

if(CONFIG_GM_HAL_RPC_SLAVE)
ugelis_sources(rpc_plugin_imp.c rpc_plugin_yolo.c rpc_plugin_qr.c)
endif()


