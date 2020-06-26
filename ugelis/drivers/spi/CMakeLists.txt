ugelis_library()

ugelis_library_sources_ifdef(CONFIG_SPI_DW		spi_dw.c)
ugelis_library_sources_ifdef(CONFIG_SPI_STM32		spi_ll_stm32.c)
ugelis_library_sources_ifdef(CONFIG_SPI_GM		spi_gm.c)

ugelis_library_sources_ifdef(CONFIG_USERSPACE		spi_handlers.c)
