ugelis_interface_library_named(TINYCBOR)

target_include_directories(TINYCBOR INTERFACE src)

ugelis_library()
ugelis_library_sources(
    src/cbor_buf_reader.c
    src/cbor_buf_writer.c
    src/cborencoder.c
    src/cborerrorstrings.c
    src/cborparser.c
)
ugelis_library_sources_ifdef(CONFIG_NEWLIB_LIBC src/cborparser_dup_string.c)

ugelis_library_link_libraries(TINYCBOR)
target_link_libraries(TINYCBOR INTERFACE ugelis_interface)
