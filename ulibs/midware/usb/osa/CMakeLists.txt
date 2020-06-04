###############################-I#####################
ugelis_include_directories(

  ${UGELIS_BASE}/subsys/usb_gm/include
  ${UGELIS_BASE}/subsys/usb_gm/device
)

#################################compile for usb specifiedf osa ##################
ugelis_sources(
    usb_osa_ugelisos.c    
    )