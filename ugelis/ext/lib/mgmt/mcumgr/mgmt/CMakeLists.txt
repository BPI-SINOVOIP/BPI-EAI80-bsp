target_include_directories(MCUMGR INTERFACE 
    include
    port/ugelis/include
)

ugelis_library_sources(
    mgmt/src/mgmt.c
)
