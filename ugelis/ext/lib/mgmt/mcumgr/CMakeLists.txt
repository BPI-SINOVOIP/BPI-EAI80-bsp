ugelis_interface_library_named(MCUMGR)

ugelis_library()
add_subdirectory(cborattr)
add_subdirectory(cmd)
add_subdirectory(mgmt)
add_subdirectory(smp)
add_subdirectory(util)

ugelis_library_link_libraries(MCUMGR)

target_link_libraries(MCUMGR INTERFACE
  ugelis_interface
  TINYCBOR
  )
