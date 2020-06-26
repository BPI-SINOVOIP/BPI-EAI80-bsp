set(BOARD f6721b_evb_qfn80pin)
include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

#set(LINK_DIR ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/speechai)
#set(SOURCE_FILE ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/speechai/speech_recognize.c)
ugelis_include_directories(
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/dev
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/easynet
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/faceai
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/inc
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/lib
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/objai
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/ui
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/ui/ugui
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/ui/mini
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app/worker
   ${UGELIS_BASE}/ugelis_example/example/ai_example/ai/src/app
)

FILE(GLOB app_sources src/app/*.c 
                      src/app/dev/*.c 
                      src/app/easynet/*.c 
                      src/app/inc/*.c 
                      src/app/lib/*.c 
                      src/app/objai/*.c 
                      src/app/ui/*.c 
                      src/app/ui/ugui/*.c 
                      src/app/worker/*.c)
target_sources(app PRIVATE ${app_sources})

#link_directories(${LINK_DIR})
#link_libraries(DSpotter2)
#add_executable(ai_prj ${SOURCE_FILE})
#target_include_directories(ai_prj GLOB app_sources)
#target_link_libraries(${LINK_DIR} DSpotter2)