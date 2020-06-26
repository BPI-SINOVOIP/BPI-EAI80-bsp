
ugelis_include_directories(

  ${UGELIS_BASE}/ext/midware/easynet_cross
  ${UGELIS_BASE}/ext/lib/openmv/include
)

add_subdirectory(lib)
add_subdirectory(hal)
add_subdirectory(debug)
add_subdirectory(midware)
