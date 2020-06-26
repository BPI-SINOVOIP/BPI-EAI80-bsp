add_subdirectory_ifdef(CONFIG_FCB  ./fcb)
add_subdirectory_ifdef(CONFIG_NVS  ./nvs)
add_subdirectory_ifdef(CONFIG_DEVRAM_FILESYSTEM  ./devram)
add_subdirectory_ifdef(CONFIG_FAT_FILESYSTEM_ELM  ./fat)
if(CONFIG_FILE_SYSTEM)
  ugelis_interface_library_named(FS)
  ugelis_link_interface_ifdef(CONFIG_FAT_FILESYSTEM_ELM ELMFAT)
  ugelis_link_interface_ifdef(CONFIG_FILE_SYSTEM_NFFS   NFFS)

  ugelis_library()
  ugelis_library_sources(fs.c)
  ugelis_library_sources_ifdef(CONFIG_FAT_FILESYSTEM_ELM fat_fs.c)
  ugelis_library_sources_ifdef(CONFIG_FILE_SYSTEM_NFFS   nffs_fs.c)
  ugelis_library_sources_ifdef(CONFIG_FILE_SYSTEM_SHELL  shell.c)

  ugelis_library_link_libraries(FS)

  target_link_libraries_ifdef(CONFIG_FAT_FILESYSTEM_ELM FS INTERFACE ELMFAT)
  target_link_libraries_ifdef(CONFIG_FILE_SYSTEM_NFFS   FS INTERFACE NFFS)
endif()


