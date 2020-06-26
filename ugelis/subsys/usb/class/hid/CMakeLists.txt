ugelis_library()

ugelis_library_include_directories(
  # USB headers
  #${UGELIS_BASE}/include/drivers/usb
  ${UGELIS_BASE}/include/usb
  ${UGELIS_BASE}/subsys/usb
  )

ugelis_library_sources(
  core.c
  )
