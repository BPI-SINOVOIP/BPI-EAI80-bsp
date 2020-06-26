add_subdirectory_ifdef(CONFIG_OV5640		ov5640)
add_subdirectory_ifdef(CONFIG_OV7725		ov7725)
add_subdirectory_ifdef(CONFIG_GC0308		gc0308)
add_subdirectory_ifdef(CONFIG_BF3007		bf3007)
add_subdirectory_ifdef(CONFIG_GC2145		gc2145)
add_subdirectory_ifdef(CONFIG_SCCB		    sccb)

ugelis_sources(                         vb_queue.c)
ugelis_sources_ifdef(CONFIG_CAMERA_GM6721X   vin_gm6721x.c)


