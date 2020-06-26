ugelis_library()
ugelis_library_sources(libc-hooks.c)

# LIBC_*_DIR may or may not have been set by the toolchain. E.g. when
# using UGELIS_TOOLCHAIN_VARIANT=cross-compile it will be either up to the
# toolchain to know where it's libc implementation is, or if it is
# unable to, it will be up to the user to specify LIBC_*_DIR vars to
# point to a newlib implementation.

if(LIBC_INCLUDE_DIR)
  ugelis_include_directories(${LIBC_INCLUDE_DIR})
endif()

if(LIBC_LIBRARY_DIR)
  set(LIBC_LIBRARY_DIR_FLAG -L${LIBC_LIBRARY_DIR})
endif()

# define __LINUX_ERRNO_EXTENSIONS__ so we get errno defines like -ESHUTDOWN
# used by the network stack
ugelis_compile_definitions(__LINUX_ERRNO_EXTENSIONS__)

ugelis_link_libraries(
  m
  c
  ${LIBC_LIBRARY_DIR_FLAG} # NB: Optional
  $<$<BOOL:${CONFIG_NEWLIB_LIBC_FLOAT_PRINTF}>:-u_printf_float>
  $<$<BOOL:${CONFIG_NEWLIB_LIBC_FLOAT_SCANF}>:-u_scanf_float>
  gcc # Lib C depends on libgcc. e.g. libc.a(lib_a-fvwrite.o) references __aeabi_idiv
  )
