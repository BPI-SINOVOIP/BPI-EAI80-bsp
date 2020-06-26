ugelis_include_directories(
    ${UGELIS_BASE}/include/rpc/core
)

if(CONFIG_GM_HAL_RPC_MASTER)
ugelis_sources(rpc_master.c)
endif()

if(CONFIG_GM_HAL_RPC_SLAVE)
ugelis_sources(rpc_slave.c)
endif()


