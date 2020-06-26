##############################-I############################
if(CONFIG_USB_MASS_STORAGE)
	ugelis_include_directories(
	    ${UGELIS_BASE}/subsys/usb_gm/device/class/msc
	)	
	
endif()


if(CONFIG_USB_VIDEO)
	ugelis_include_directories(
	    ${UGELIS_BASE}/subsys/usb_gm/device/class/video
	)

endif()

if(CONFIG_USB_AUDIO)
	ugelis_include_directories(
	    ${UGELIS_BASE}/subsys/usb_gm/device/class/audio
	)	

endif()

if(CONFIG_USB_HID_MOUSE)
	ugelis_include_directories(
	    ${UGELIS_BASE}/subsys/usb_gm/device/class/hid
	)	

endif()

if(CONFIG_USB_HID_CUSTOM)
	ugelis_include_directories(
	    ${UGELIS_BASE}/subsys/usb_gm/device/class/hid
	)	

endif()

ugelis_sources(
    usb_device_dci.c    
	usb_device_khci.c
	usb_device_ch9.c
	usb_device_class.c
	
    )
	
add_subdirectory(class)