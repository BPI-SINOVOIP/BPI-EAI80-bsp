ugelis_sources(
  settings_store.c
  settings.c
  settings_init.c
  settings_line.c
  )

ugelis_sources_ifdef(CONFIG_SETTINGS_FS settings_file.c)
ugelis_sources_ifdef(CONFIG_SETTINGS_FCB settings_fcb.c)
