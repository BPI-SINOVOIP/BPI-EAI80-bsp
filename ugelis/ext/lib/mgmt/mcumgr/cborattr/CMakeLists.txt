target_include_directories(MCUMGR INTERFACE 
    include
)

ugelis_library_sources(
    cborattr/src/cborattr.c
)
