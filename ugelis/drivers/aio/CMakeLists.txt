ugelis_library()

ugelis_library_sources_ifdef(CONFIG_AIO_COMPARATOR_QMSI aio_comparator_qmsi.c)
ugelis_library_sources_ifdef(CONFIG_USERSPACE           aio_comparator_handlers.c)
