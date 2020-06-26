# use "Generic" as CMAKE_SYSTEM_NAME

if (WITH_UGELIS)
  set (CMAKE_SYSTEM_NAME "Generic"        CACHE STRING "")
  string (TOLOWER "uGelis"                PROJECT_SYSTEM)
  string (TOUPPER "uGelis"                PROJECT_SYSTEM_UPPER)
  if (NOT WITH_UGELIS_LIB)
    include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
  endif ()
  if (CONFIG_CPU_CORTEX_M)
    set (MACHINE "cortexm" CACHE STRING "")
  endif (CONFIG_CPU_CORTEX_M)
endif (WITH_UGELIS)
