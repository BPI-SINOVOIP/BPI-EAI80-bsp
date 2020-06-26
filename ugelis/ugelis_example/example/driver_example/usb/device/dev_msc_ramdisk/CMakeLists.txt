set(BOARD f6721b_evb_176pin)
include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

ugelis_include_directories(
	    ${UGELIS_BASE}/subsys/usb_gm/include
		${UGELIS_BASE}/subsys/usb_gm/osa
		${UGELIS_BASE}/subsys/usb_gm/device
		${UGELIS_BASE}/subsys/usb_gm/class/msc
	
	)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
