ugelis_include_directories(${UGELIS_BASE}/ext/lib/jerry-libm)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} filelist)
target_sources(app PRIVATE ${filelist})


