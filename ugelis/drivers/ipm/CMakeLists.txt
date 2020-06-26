ugelis_library()

ugelis_library_sources_ifdef(CONFIG_IPM_MCUX   ipm_mcux.c)
ugelis_library_sources_ifdef(CONFIG_IPM_QUARK_SE ipm_quark_se.c)

ugelis_library_sources_ifdef(CONFIG_USERSPACE   ipm_handlers.c)
