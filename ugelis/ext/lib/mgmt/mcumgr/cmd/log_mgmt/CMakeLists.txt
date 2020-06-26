target_include_directories(MCUMGR INTERFACE 
    include
)

ugelis_library_sources(
    cmd/log_mgmt/port/ugelis/src/ugelis_log_mgmt.c
    cmd/log_mgmt/src/log_mgmt.c
    cmd/log_mgmt/src/stubs.c
)
