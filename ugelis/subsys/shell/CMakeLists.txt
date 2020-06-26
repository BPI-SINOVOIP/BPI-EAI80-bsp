ugelis_include_directories_ifdef(CONFIG_CONSOLE_SHELL
  ${UGELIS_BASE}/include/drivers
  )

ugelis_sources(
  shell_service.c
  shell.c
  )

add_subdirectory(modules)
