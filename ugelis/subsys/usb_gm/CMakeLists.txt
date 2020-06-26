add_subdirectory(osa)

if(CONFIG_GM_USB_DEVICE_STACK)
  add_subdirectory(device)
endif()

if(CONFIG_GM_USB_HOST_STACK)
  add_subdirectory(host)
endif()