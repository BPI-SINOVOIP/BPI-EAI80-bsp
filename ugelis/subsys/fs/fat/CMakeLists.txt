add_library(ELMFAT INTERFACE)

target_include_directories(ELMFAT INTERFACE include)

ugelis_library()
ugelis_library_sources(
    ff.c
    zfs_diskio.c
    )
ugelis_library_link_libraries(ELMFAT)
target_link_libraries(ELMFAT INTERFACE ugelis_interface)
