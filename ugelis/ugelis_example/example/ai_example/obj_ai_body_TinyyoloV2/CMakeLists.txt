set(BOARD f6721b_evb_qfn80pin)
include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

ugelis_include_directories(

   ${UGELIS_BASE}/ugelis_example/example/ai_example/obj_ai_body_TinyyoloV2/src/inc/cam
   ${UGELIS_BASE}/ugelis_example/example/ai_example/obj_ai_body_TinyyoloV2/src/inc/common
   ${UGELIS_BASE}/ugelis_example/example/ai_example/obj_ai_body_TinyyoloV2/src/inc/lcd   
   ${UGELIS_BASE}/ugelis_example/example/ai_example/obj_ai_body_TinyyoloV2/src
)

FILE(GLOB app_sources src/*.c src/camera/*.c src/lcd/*.c)

target_sources(app PRIVATE ${app_sources})