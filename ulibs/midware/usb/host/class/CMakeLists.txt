###############################-I#####################
ugelis_include_directories(

  ${UGELIS_BASE}/subsys/usb_gm/include
  ${UGELIS_BASE}/subsys/usb_gm/host
  ${UGELIS_BASE}/subsys/usb_gm/osa
  ${UGELIS_BASE}/subsys/usb_gm/host/class

)

if(CONFIG_USB_MASS_STORAGE)
  add_subdirectory(msc)
endif()

if(CONFIG_USB_HID_MOUSE)
  add_subdirectory(hid)
endif()

if(CONFIG_USB_AUDIO)
  add_subdirectory(audio)
endif()

ugelis_sources(
      usb_host_hub.c
	  usb_host_hub_app.c	  
    )