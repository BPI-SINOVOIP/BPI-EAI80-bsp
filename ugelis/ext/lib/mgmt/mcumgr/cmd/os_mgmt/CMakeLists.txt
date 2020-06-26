target_include_directories(MCUMGR INTERFACE 
    include
)

ugelis_library_sources(
    cmd/os_mgmt/port/ugelis/src/ugelis_os_mgmt.c
    cmd/os_mgmt/src/os_mgmt.c
    cmd/os_mgmt/src/stubs.c
)
