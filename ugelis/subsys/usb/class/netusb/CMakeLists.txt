ugelis_library()

ugelis_library_include_directories(
  # USB headers
  ${UGELIS_BASE}/include/drivers/usb
  ${UGELIS_BASE}/include/usb/
  ${UGELIS_BASE}/subsys/usb/
  ..

  # IP headers
  ${UGELIS_BASE}/subsys/net/ip
  )

ugelis_library_sources(
  netusb.c
  )

ugelis_library_sources_ifdef(
  CONFIG_USB_DEVICE_NETWORK_ECM
  function_ecm.c
  )

ugelis_library_sources_ifdef(
  CONFIG_USB_DEVICE_NETWORK_RNDIS
  function_rndis.c
  )

ugelis_library_sources_ifdef(
  CONFIG_USB_DEVICE_NETWORK_EEM
  function_eem.c
  )
