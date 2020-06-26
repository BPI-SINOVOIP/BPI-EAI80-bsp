set(BOARD f6721b_evb_176pin)
include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

ugelis_include_directories(

   ${UGELIS_BASE}/samples/gm6721_prj/example/zlib/src/zlib
)

FILE(GLOB app_sources src/*.c src/zlib/*.c)

target_sources(app PRIVATE ${app_sources})
