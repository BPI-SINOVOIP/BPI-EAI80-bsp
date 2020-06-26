ugelis_sources_ifdef(CONFIG_DISK_ACCESS disk_access.c)
ugelis_sources_ifdef(CONFIG_DISK_ACCESS_FLASH disk_access_flash.c)
ugelis_sources_ifdef(CONFIG_DISK_ACCESS_RAM disk_access_ram.c)
ugelis_sources_ifdef(CONFIG_DISK_ACCESS_MMC disk_access_mmc.c)

ugelis_include_directories_ifdef(CONFIG_FAT_FILESYSTEM_ELM
		${UGELIS_BASE}/subsys/fs/fat/include
)

ugelis_include_directories_ifdef (CONFIG_DISK_ACCESS_USB
		${UGELIS_BASE}/subsys/usb_gm/include
		${UGELIS_BASE}/subsys/usb_gm/osa
		${UGELIS_BASE}/subsys/usb_gm/host
		${UGELIS_BASE}/subsys/usb_gm/host/class/msc
	
	)


	
ugelis_sources_ifdef(CONFIG_DISK_ACCESS_USB disk_access_usb.c gm_usb_disk_ugelis.c)