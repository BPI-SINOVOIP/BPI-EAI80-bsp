target_include_directories(MCUMGR INTERFACE 
    include
)

ugelis_library_sources(
    smp/src/smp.c
)
