target_include_directories(MCUMGR INTERFACE 
    include
)

ugelis_library_sources(
    cmd/stat_mgmt/port/ugelis/src/ugelis_stat_mgmt.c
    cmd/stat_mgmt/src/stat_mgmt.c
    cmd/stat_mgmt/src/stubs.c
)
