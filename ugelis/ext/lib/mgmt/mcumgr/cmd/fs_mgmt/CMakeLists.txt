target_include_directories(MCUMGR INTERFACE 
    include
)

ugelis_library_sources(
    cmd/fs_mgmt/port/ugelis/src/ugelis_fs_mgmt.c
    cmd/fs_mgmt/src/fs_mgmt.c
    cmd/fs_mgmt/src/stubs.c
)
