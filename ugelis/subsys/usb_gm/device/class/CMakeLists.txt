##############################-I############################
if(CONFIG_USB_VIDEO)	
	add_subdirectory(video)
endif()

if(CONFIG_USB_AUDIO)		
	add_subdirectory(audio)
endif()

if(CONFIG_USB_HID_MOUSE)		
	add_subdirectory(hid)
endif()

if(CONFIG_USB_HID_CUSTOM)		
	add_subdirectory(hid)
endif()

if(CONFIG_USB_MASS_STORAGE)		
	add_subdirectory(msc)
endif()
