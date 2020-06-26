ugelis_sources(line_fifo.c)
ugelis_sources_ifdef(CONFIG_CONSOLE_GETCHAR getchar.c)
ugelis_sources_ifdef(CONFIG_CONSOLE_GETLINE getline.c)
